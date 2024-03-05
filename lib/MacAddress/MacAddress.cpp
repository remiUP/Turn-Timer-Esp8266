#include "MacAddress.h"

size_t MacAddress::printTo(Print &p) const
{
	size_t r = 0;
	for (int i = 0; i < 6; i++)
	{
		r += p.print(mac[i], HEX);
		if (i < 5)
		{
			r += p.print(":");
		}
	}
	return r;
};

uint8_t *MacAddress::getMacAddress()
{
	return mac;
}

bool MacAddress::isNull()
{
	for (int i = 0; i < 6; i++)
	{
		if (mac[i] != 0)
		{
			return false;
		}
	}
	return true;
};

void MacAddress::setMacAddress(uint8_t *mac)
{
	for (int i = 0; i < 6; i++)
	{
		this->mac[i] = mac[i];
	}
};