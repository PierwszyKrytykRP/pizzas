#include <gtest/gtest.h>
#include <string>
#include "Funghi.hpp"
#include "Margherita.hpp"
#include "Pizzeria.hpp"
#include "Timer.hpp"
#include "mocks/PizzaMock.hpp"
#include "mocks/TimerMock.hpp"

using namespace std;
using namespace ::testing;

struct PizzeriaTest : public ::testing::Test {
public:
    StrictMock<TimeMock> timerMock{};
    Pizzeria pizzeria = Pizzeria("dummyName", timerMock);
};

TEST_F(PizzeriaTest, priceForMargherita25AndFunghi30ShouldBe55) {
    // Given
    Pizzas pizzas = {new Margherita{25.0}, new Funghi{30.0}};

    // When
    auto orderId = pizzeria.makeOrder(pizzas);
    auto price = pizzeria.calculatePrice(orderId);

    // Then
    ASSERT_EQ(55, price);
}

TEST_F(PizzeriaTest, bakeDummyPizza) {
    // Given
    Pizzas pizzas = {new PizzaDummy{}};
    EXPECT_CALL(timerMock, sleep_for(minutes(0))).Times(1);

    // When
    auto orderId = pizzeria.makeOrder(pizzas);
    pizzeria.bakePizzas(orderId);
}

TEST_F(PizzeriaTest, completeOrderWithStubPizza) {
    // Given
    Pizzas pizzas = {new PizzaStub{"STUB"}};
    EXPECT_CALL(timerMock, sleep_for(minutes(1))).Times(1);

    // When
    auto orderId = pizzeria.makeOrder(pizzas);
    pizzeria.bakePizzas(orderId);
    pizzeria.completeOrder(orderId);
}

TEST_F(PizzeriaTest, calculatePriceForPizzaMock) {
    // Given
    PizzaMock* mock = new PizzaMock{};
    Pizzas pizzas = {mock};
    constexpr double mockPrice = 40.0;
    EXPECT_CALL(*mock, getPrice()).WillOnce(Return(mockPrice));

    // When
    auto orderId = pizzeria.makeOrder(pizzas);
    auto price = pizzeria.calculatePrice(orderId);

    // Then
    ASSERT_EQ(mockPrice, price);

    delete mock;
}

TEST_F(PizzeriaTest, testMainFunctionalityForPizzaVariety) {
    //Given
    PizzaStub stub{"stub"};
    StrictMock<PizzaMock> strictPizza{};
    NiceMock<PizzaMock> nicePizza{};
    Pizzas pizzas = {&stub, &strictPizza, &nicePizza};
    constexpr double strictPrice = 20.0;
    constexpr double nicePrice = 40.0;
    constexpr auto strictTime = minutes(2);
    constexpr auto niceTime = minutes(3);
    constexpr auto stubTime = minutes(1);

    EXPECT_CALL(strictPizza, getName()).WillOnce(Return("strictPizza"));
    EXPECT_CALL(strictPizza, getPrice()).WillOnce(Return(strictPrice));
    EXPECT_CALL(strictPizza, getBakingTime()).WillOnce(Return(strictTime));

    EXPECT_CALL(nicePizza, getName()).WillOnce(Return("nicePizza"));
    EXPECT_CALL(nicePizza, getPrice()).WillOnce(Return(nicePrice));
    EXPECT_CALL(nicePizza, getBakingTime()).WillOnce(Return(niceTime));

    EXPECT_CALL(timerMock, sleep_for(stubTime)).Times(1);
    EXPECT_CALL(timerMock, sleep_for(strictTime)).Times(1);
    EXPECT_CALL(timerMock, sleep_for(niceTime)).Times(1);

    //When
    auto orderId = pizzeria.makeOrder(pizzas);
    auto price = pizzeria.calculatePrice(orderId);
    pizzeria.bakePizzas(orderId);
    pizzeria.completeOrder(orderId);

    //Then
    ASSERT_EQ(nicePrice + strictPrice, price);
}
