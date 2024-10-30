#pragma once
#include "configuration.hpp"

struct WKernel
{
	float h = conf::h;

	float W(float d);
	float dW(float d);
};

float WKernel::W(float d)
{
	if (d <= 2.f * h)
	{
		float S = 14.f / 30.f * 3.14159 * h * h;
		float firstPart = 1.f / 6.f * (2 - d / h) * (2 - d / h) * (2 - d / h);
		float secondPart = 4.f / 6.f * (1 - d / h) * (1 - d / h) * (1 - d / h);
		return (d <= h) ? 1.f/S * ( firstPart - secondPart ) : 1.f / S * firstPart;
	}
	return 0;
}

float WKernel::dW(float d)
{
	if (d <= 2.f * h)
	{
		float S = 14.f / 30.f * 3.14159 * h * h;
		float firstPart = -1.f / (2.f * h) * (2 - d / h) * (2 - d / h);
		float secondPart = -2.f / h * (1 - d / h) * (1 - d / h);
		return (d <= h) ? 1.f / S * (firstPart - secondPart) : 1.f / S * firstPart;
	}
	return 0;
}