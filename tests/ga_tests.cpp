#include <catch2/catch_test_macros.hpp>

#include "../genetic_algorithm/genetic_algorithm.hpp"
#include "../genetic_algorithm/standard_stop_criterion.hpp"
#include "../genetic_algorithm/communicating_fitness_counter.hpp"

#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

using ga::core::Chromosome;
using ga::core::CommunicatingFitnessCounter;
using ga::core::Communication;
using ga::core::Crossover;
using ga::core::FitnessCounter;
using ga::core::GeneticAlgorithm;
using ga::core::Mutation;
using ga::core::Population;
using ga::core::Selection;
using ga::core::StandardStopCriterion;


struct Select : Selection<int> {
    std::string& trace;

    Select(std::string& trace) : trace(trace) {}

    Population<int> select(const Population<int>& p) override {
        trace += 'S';
        for (std::size_t i = 0; i < p.size(); ++i) {
            REQUIRE(p[i].fitness.has_value());
        }
        return p;
    }
};

struct Cross : Crossover<int> {
    std::string& trace;

    Cross(std::string& trace) : trace(trace) {}

    void crossover(Population<int>& p) override {
        trace += 'C';
        for (std::size_t i = 0; i < p.size(); ++i) {
            REQUIRE_FALSE(p[i].fitness.has_value());
            p[i].genome += 10;
        }
    }
};

struct Mutate : Mutation<int> {
    std::string& trace;
    int delta;

    Mutate(std::string& trace, int delta) : trace(trace), delta(delta) {}

    void mutate(Population<int>& p) override {
        trace += 'M';
        for (std::size_t i = 0; i < p.size(); ++i) {
            REQUIRE_FALSE(p[i].fitness.has_value());
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
        REQUIRE_FALSE(c.fitness.has_value());
        return c.genome + communication_time(Transfer{1});
    }
};

struct BadSelection : Selection<int> {
    Population<int> select(const Population<int>&) override {
        return {};
    }
};

struct NaNEvaluator : FitnessCounter<int> {
    double count(const Chromosome<int>&) override {
        return std::numeric_limits<double>::quiet_NaN();
    }
};

struct TestContext {
    Population<int> input;
    std::string trace;
    Link link;
    int mutation_delta = 1;

    TestContext(bool fill_population = true) {
        if (fill_population) {
            input.add({1, std::nullopt});
            input.add({4, std::nullopt});
        }
    }
};

enum class Mode {
    Normal,
    Maximize,
    EmptySelection,
    NaNFitness
};


GeneticAlgorithm<int> make_ga(TestContext& context,
                              const StandardStopCriterion<int>& stop,
                              Mode mode = Mode::Normal) {
    std::unique_ptr<Selection<int>> selection =
        std::make_unique<Select>(context.trace);
    std::unique_ptr<FitnessCounter<int>> evaluator =
        std::make_unique<Evaluate>(context.link);

    if (mode == Mode::EmptySelection) {
        selection = std::make_unique<BadSelection>();
    }
    if (mode == Mode::NaNFitness) {
        evaluator = std::make_unique<NaNEvaluator>();
    }

    std::function<bool(double, double)> better = std::less<double>{};
    if (mode == Mode::Maximize) {
        better = std::greater<double>{};
    }

    return GeneticAlgorithm<int>(
        context.input,
        std::move(selection),
        std::make_unique<Mutate>(context.trace, context.mutation_delta),
        std::make_unique<Cross>(context.trace),
        std::move(evaluator),
        std::make_unique<StandardStopCriterion<int>>(stop),
        better
    );
}

TEST_CASE("Generation order and elitism", "[ga]") {
    TestContext context;
    StandardStopCriterion<int> stop(10, 2);
    auto ga = make_ga(context, stop);

    auto best = ga.run();

    REQUIRE(best.genome == 1);
    REQUIRE(best.fitness == 3);
    REQUIRE(context.trace == "SCMSCM");
    REQUIRE(ga.stats().generation == 2);
    REQUIRE(ga.stats().generations_without_improvement == 2);
    REQUIRE(context.input[0].genome == 1);
    REQUIRE_FALSE(context.input[0].fitness.has_value());

    bool elite_present = false;
    for (std::size_t i = 0; i < ga.current_population().size(); ++i) {
        if (ga.current_population()[i].fitness == 3) {
            elite_present = true;
        }
    }
    REQUIRE(elite_present);
}

TEST_CASE("Repeated run resets statistics", "[ga]") {
    TestContext context;
    StandardStopCriterion<int> stop(10, 2);
    auto ga = make_ga(context, stop);

    ga.run();
    context.trace.clear();
    ga.run();

    REQUIRE(ga.stats().generation == 2);
    REQUIRE(context.trace == "SCMSCM");
}

TEST_CASE("Zero generation limit", "[ga][stop]") {
    TestContext context;
    StandardStopCriterion<int> stop(0);
    auto ga = make_ga(context, stop);

    auto best = ga.run();

    REQUIRE(best.fitness == 3);
    REQUIRE(context.trace.empty());
    REQUIRE(ga.stats().generation == 0);
}

TEST_CASE("Communication time", "[ga][communication]") {
    TestContext context;
    StandardStopCriterion<int> stop(0);
    auto ga = make_ga(context, stop);

    REQUIRE(ga.run().fitness == 3);

    context.link.duration = 7;
    REQUIRE(ga.run().fitness == 8);

    context.link.duration = -1;
    REQUIRE_THROWS_AS(ga.run(), std::domain_error);

    context.link.duration = std::numeric_limits<double>::infinity();
    REQUIRE_THROWS_AS(ga.run(), std::domain_error);
}

TEST_CASE("Improvement and negative fitness", "[ga]") {
    TestContext context;
    context.link.duration = 0;
    context.mutation_delta = -12;
    StandardStopCriterion<int> stop(2);
    auto ga = make_ga(context, stop);

    REQUIRE(ga.run().fitness == -3);
    REQUIRE(ga.stats().generations_without_improvement == 0);
}

TEST_CASE("Maximization", "[ga]") {
    TestContext context;
    context.link.duration = 0;
    StandardStopCriterion<int> stop(2);
    auto ga = make_ga(context, stop, Mode::Maximize);

    REQUIRE(ga.run().fitness == 26);
}

TEST_CASE("Empty population", "[ga][errors]") {
    TestContext context(false);
    StandardStopCriterion<int> stop(2);

    REQUIRE_THROWS_AS(make_ga(context, stop), std::invalid_argument);
}

TEST_CASE("Population size contract", "[ga][errors]") {
    TestContext context;
    StandardStopCriterion<int> stop(2);
    auto ga = make_ga(context, stop, Mode::EmptySelection);

    REQUIRE_THROWS_AS(ga.run(), std::logic_error);
}

TEST_CASE("Non-finite fitness", "[ga][errors]") {
    TestContext context;
    StandardStopCriterion<int> stop(2);
    auto ga = make_ga(context, stop, Mode::NaNFitness);

    REQUIRE_THROWS_AS(ga.run(), std::domain_error);
}

TEST_CASE("Convergence", "[stop]") {
    StandardStopCriterion<int> enabled(100, 0, true);
    StandardStopCriterion<int> disabled(100, 0, false);
    Population<int> population;
    population.add({1, 3});
    population.add({2, 3});

    REQUIRE(enabled.check(population));
    REQUIRE_FALSE(disabled.check(population));

    population[1].fitness = 4;
    REQUIRE_FALSE(enabled.check(population));

    population[1].fitness.reset();
    REQUIRE_FALSE(enabled.check(population));
}
