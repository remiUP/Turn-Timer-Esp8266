#pragma once
#include <stdint.h>
#include <Printable.h>
#include <Print.h>

class MacAddress : public Printable
{
private:
	uint8_t mac[6] = {0};

public:
	bool operator==(const MacAddress &other)
	{
		for (int i = 0; i < 6; i++)
		{
			if (mac[i] != other.mac[i])
			{
				return false;
			}
		}
		return true;
	};
	bool isNull();
	bool operator!=(const MacAddress &other)
	{
		return !(*this == other);
	};
	operator uint8_t *()
	{
		return mac;
	};
	size_t printTo(Print &p) const;
	uint8_t *getMacAddress();
	void setMacAddress(uint8_t *);
	void setMacAddressBroadcast();
};