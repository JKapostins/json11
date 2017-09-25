#include <Core/Memory/MallocArena.h>
#include <gtest/gtest.h>
#include <json11/json11.hpp>

	namespace json11
	{
		using eastl::string;
		TEST(json11, simple_test)
		{
			const string simple_test = R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null]})";

			string err;
			const auto json = Json::parse(simple_test, err);

			std::cout << "k1: " << json["k1"].string_value().c_str() << "\n";
			std::cout << "k3: " << json["k3"].dump().c_str() << "\n";

			for (auto &k : json["k3"].array_items())
			{
				std::cout << "    - " << k.dump().c_str() << "\n";
			}
			EXPECT_TRUE(true);
		}

	}
