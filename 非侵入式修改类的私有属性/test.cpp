#include <iostream>


class Bank {
private:
    double money = 9999;

public:
    void check() const {
        std::cout << "money: " << money << std::endl;
    }
};


template <auto M>
struct Tunnel;

template <typename T, typename U, T U::* M>
struct Tunnel<M> {
    friend T& sneak(U& u) {
        return u.*M;
    }
};


template
struct Tunnel<&Bank::money>;
double& sneak(Bank&);


int main() {
    Bank bank;

    std::cout << "[before]";
    bank.check();

    sneak(bank) = 1;
    std::cout << "[after]";
    bank.check();

    return 0;
}

