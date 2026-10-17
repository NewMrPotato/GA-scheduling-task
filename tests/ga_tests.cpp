
#include "genetic_algorithm.hpp"
#include "standard_stop_criterion.hpp"
#include "communicating_fitness_counter.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <limits>
#include <string>


void require(bool condition, const std::string& message) {
	if (!condition) {
		throw std::runtime_error(message);
	}
}

struct Select : Selection<int> {
	std::string& trace;

	explicit Select(std::string& trace) : trace(trace) {}

	Population<int> select(const Population<int>& p) override {
		trace += 'S';
		for (std::size_t i = 0; i < p.size(); ++i) {
			require(p[i].fitness.has_value(), "Selection received unevaluated fitness");
		}

		return p;
	}
};

struct Cross : Crossover<int> {
	std::string& trace;

	explicit Cross(std::string& trace) : trace(trace) {}

	void crossover(Population<int>& p) override {
		trace += 'C';
		for (std::size_t i = 0; i < p.size(); ++i) {
			require(!p[i].fitness, "Operator received stale fitness");
			p[i].genome += 10;
		}
	}
};

struct Mutate : Mutation<int> {
	std::string& trace;
	int delta = 1;

	explicit Mutate(std::string& trace) : trace(trace) {}

	void mutate(Population<int>& p) override {
		trace += 'M';
		for (std::size_t i = 0; i < p.size(); ++i) {
			require(!p[i].fitness, "Operator received stale fitness");
			p[i].genome += delta;
		}
	}
};

struct Transfer {
	double bytes;
};

struct Link : Communication<Transfer> {
	double duration = 2;
	double time(const Transfer&) const override {
		return duration;
	}
};

struct Evaluate : CommunicatingFitnessCounter<int, Transfer> {
	using CommunicatingFitnessCounter::CommunicatingFitnessCounter;
	double count(const Chromosome<int>& c) override {
		require(!c.fitness, "Evaluator received stale fitness");

		return c.genome + communication_time(Transfer{1});
	}
};

template <typename Error, typename Action>
void expect_exception(Action action, const std::string& message) {
	try {
		action();
	} catch (const Error&) {
		return;
	}

	throw std::runtime_error(message);
}

template <typename S, typename M, typename C, typename E, typename T>
GeneticAlgorithm<int> make_ga(const Population<int>& p, const S& s, const M& m,
							 const C& c, const E& e, const T& t,
							 std::function<bool(double, double)> better = std::less<double>{}) {
	return GeneticAlgorithm<int>(p, std::make_unique<S>(s), std::make_unique<M>(m),
		std::make_unique<C>(c), std::make_unique<E>(e), std::make_unique<T>(t), better);
}

struct TestContext {
	Population<int> input;
	std::string trace;
	Select select;
	Cross cross;
	Mutate mutate;
	Link link;
	Evaluate evaluate;

	TestContext()
		: select(trace), cross(trace), mutate(trace), evaluate(link) {
		input.add({1, std::nullopt});
		input.add({4, std::nullopt});
	}
};

void test_generation_order_and_elitism() {
	TestContext context;
	StandardStopCriterion<int> stop(10, 2);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, context.evaluate, stop);

	const auto best = ga.run();

	require(best.genome == 1 && best.fitness == 3, "Elite was lost");
	require(context.trace == "SCMSCM", "Incorrect operator order");
	require(ga.stats().generation == 2, "Incorrect generation count");
	require(ga.stats().generations_without_improvement == 2, "Incorrect stagnation count");
	require(context.input[0].genome == 1 && !context.input[0].fitness,
		"Input population was modified");

	bool elite_present = false;

	for (std::size_t i = 0; i < ga.current_population().size(); ++i) {
		if (ga.current_population()[i].fitness == 3) {
			elite_present = true;
		}
	}

	require(elite_present, "Elite was not restored to the population");
}

void test_repeated_run_resets_statistics() {
	TestContext context;
	StandardStopCriterion<int> stop(10, 2);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, context.evaluate, stop);

	ga.run();
	context.trace.clear();
	ga.run();

	require(ga.stats().generation == 2, "Generation counter was not reset");
	require(context.trace == "SCMSCM", "Repeated run stopped too early");
}

void test_zero_generation_limit() {
	TestContext context;
	StandardStopCriterion<int> stop(0);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, context.evaluate, stop);

	const auto best = ga.run();

	require(best.fitness == 3, "Initial population was not evaluated");
	require(context.trace.empty(), "Operators ran with zero generation limit");
	require(ga.stats().generation == 0, "Generation zero was not preserved");
}

void test_communication_time() {
	TestContext context;
	StandardStopCriterion<int> stop(0);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, context.evaluate, stop);

	require(ga.run().fitness == 3, "Incorrect initial communication cost");

	context.link.duration = 7;
	require(ga.run().fitness == 8, "Communication time did not affect fitness");

	context.link.duration = -1;
	expect_exception<std::domain_error>([&] { ga.run(); },
		"Negative communication duration was accepted");

	context.link.duration = std::numeric_limits<double>::infinity();
	expect_exception<std::domain_error>([&] { ga.run(); },
		"Infinite communication duration was accepted");
}

void test_improvement_and_negative_fitness() {
	TestContext context;
	context.link.duration = 0;
	context.mutate.delta = -12;
	StandardStopCriterion<int> stop(2);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, context.evaluate, stop);

	require(ga.run().fitness == -3, "Improved negative fitness was not retained");
	require(ga.stats().generations_without_improvement == 0,
		"Improvement did not reset stagnation");
}

void test_maximization() {
	TestContext context;
	context.link.duration = 0;
	StandardStopCriterion<int> stop(2);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, context.evaluate, stop, std::greater<double>{});

	require(ga.run().fitness == 26, "Maximization returned the wrong result");
}

void test_empty_population() {
	TestContext context;
	StandardStopCriterion<int> stop(2);

	expect_exception<std::invalid_argument>([&] {
		auto ga = make_ga(Population<int>{}, context.select, context.mutate,
			context.cross, context.evaluate, stop);
	}, "Empty population was accepted");
}

struct BadSelection : Selection<int> {
	Population<int> select(const Population<int>&) override {
		return {};
	}
};

void test_population_size_contract() {
	TestContext context;
	BadSelection select;
	StandardStopCriterion<int> stop(2);
	auto ga = make_ga(context.input, select, context.mutate,
		context.cross, context.evaluate, stop);

	expect_exception<std::logic_error>([&] { ga.run(); },
		"Selection was allowed to change population size");
}

struct NaNEvaluator : FitnessCounter<int> {
	double count(const Chromosome<int>&) override {
		return std::numeric_limits<double>::quiet_NaN();
	}
};

void test_non_finite_fitness() {
	TestContext context;
	NaNEvaluator evaluate;
	StandardStopCriterion<int> stop(2);
	auto ga = make_ga(context.input, context.select, context.mutate,
		context.cross, evaluate, stop);

	expect_exception<std::domain_error>([&] { ga.run(); }, "NaN fitness was accepted");
}

void test_convergence() {
	StandardStopCriterion<int> enabled(100, 0, true);
	StandardStopCriterion<int> disabled(100, 0, false);
	Population<int> population;
	population.add({1, 3});
	population.add({2, 3});

	require(enabled.check(population), "Equal fitness did not trigger convergence");
	require(!disabled.check(population), "Disabled convergence was checked");

	population[1].fitness = 4;
	require(!enabled.check(population), "Unequal fitness triggered convergence");

	population[1].fitness.reset();
	require(!enabled.check(population), "Missing fitness triggered convergence");
}

int run_test(const char* name, const std::function<void()>& test) {
	try {
		test();
		std::cout << "[PASS] " << name << '\n';
		return 0;
	} catch (const std::exception& error) {
		std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
		return 1;
	}
}

int main() {
	int failures = 0;

	failures += run_test("generation order and elitism", test_generation_order_and_elitism);
	failures += run_test("repeated run", test_repeated_run_resets_statistics);
	failures += run_test("zero generation limit", test_zero_generation_limit);
	failures += run_test("communication time", test_communication_time);
	failures += run_test("improvement", test_improvement_and_negative_fitness);
	failures += run_test("maximization", test_maximization);
	failures += run_test("empty population", test_empty_population);
	failures += run_test("population size contract", test_population_size_contract);
	failures += run_test("non-finite fitness", test_non_finite_fitness);
	failures += run_test("convergence", test_convergence);

	std::cout << "Failures: " << failures << '\n';
	return failures == 0 ? 0 : 1;
}
