//
// Created by Admin on 2024/10/31.
//
#include "myFuntion.h"
double caps::generateGaussianNoise(double mean, double stddev)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(mean, stddev);  // 正态分布
    return dis(gen);
}
