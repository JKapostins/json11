#include <Core/Memory/MallocArena.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/set.h>
#include <EASTL/vector.h>
#include <gtest/gtest.h>
#include <json11/json11.hpp>

	namespace json11
	{
		using eastl::string;
		TEST(json11, simple)
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

			EXPECT_TRUE(json["k2"].is_number());
			EXPECT_TRUE(json["k2"].int_value() == 42);
			EXPECT_TRUE(json["k3"].is_array());
			EXPECT_FALSE(json["k1"].is_array());
			EXPECT_TRUE(json["k1"].is_string());
		}

		TEST(json11, comments)
		{
			string comment_test = R"({
			  // comment /* with nested comment */
			  "a": 1,
			  // comment
			  // continued
			  "b": "text",
			  /* multi
				 line
				 comment
				// line-comment-inside-multiline-comment
			  */
			  // and single-line comment
			  // and single-line comment /* multiline inside single line */
			  "c": [1, 2, 3]
			  // and single-line comment at end of object
			})";

			string err_comment;
			auto json_comment = Json::parse(
				comment_test, err_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(!json_comment.is_null());
			EXPECT_TRUE(err_comment.empty());

			comment_test = "{\"a\": 1}//trailing line comment";
			json_comment = Json::parse(
				comment_test, err_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(!json_comment.is_null());
			EXPECT_TRUE(err_comment.empty());

			comment_test = "{\"a\": 1}/*trailing multi-line comment*/";
			json_comment = Json::parse(
				comment_test, err_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(!json_comment.is_null());
			EXPECT_TRUE(err_comment.empty());

			string failing_comment_test = "{\n/* unterminated comment\n\"a\": 1,\n}";
			string err_failing_comment;
			auto json_failing_comment = Json::parse(
				failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(json_failing_comment.is_null());
			EXPECT_TRUE(!err_failing_comment.empty());

			failing_comment_test = "{\n/* unterminated trailing comment }";
			json_failing_comment = Json::parse(
				failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(json_failing_comment.is_null());
			EXPECT_TRUE(!err_failing_comment.empty());

			failing_comment_test = "{\n/ / bad comment }";
			json_failing_comment = Json::parse(
				failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(json_failing_comment.is_null());
			EXPECT_TRUE(!err_failing_comment.empty());

			failing_comment_test = "{// bad comment }";
			json_failing_comment = Json::parse(
				failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(json_failing_comment.is_null());
			EXPECT_TRUE(!err_failing_comment.empty());

			failing_comment_test = "{\n\"a\": 1\n}/";
			json_failing_comment = Json::parse(
				failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(json_failing_comment.is_null());
			EXPECT_TRUE(!err_failing_comment.empty());

			failing_comment_test = "{/* bad\ncomment *}";
			json_failing_comment = Json::parse(
				failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
			EXPECT_TRUE(json_failing_comment.is_null());
			EXPECT_TRUE(!err_failing_comment.empty());
		}

		TEST(json11, compare)
		{
			eastl::list<int> l1{ 1, 2, 3 };
			eastl::vector<int> l2{ 1, 2, 3 };
			eastl::set<int> l3{ 1, 2, 3 };
			EXPECT_TRUE(Json(l1) == Json(l2));
			EXPECT_TRUE(Json(l2) == Json(l3));

			eastl::map<string, string> m1{ { "k1", "v1" },{ "k2", "v2" } };
			eastl::map<string, string> m2{ { "k1", "v1" },{ "k2", "v2" } };
			EXPECT_TRUE(Json(m1) == Json(m2));
		}

		TEST(json11, literals)
		{
			const string simple_test =
				R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null]})";

			string err;
			const auto json = Json::parse(simple_test, err);

			const Json obj = Json::object({
				{ "k1", "v1" },
				{ "k2", 42.0 },
				{ "k3", Json::array({ "a", 123.0, true, false, nullptr }) },
			});

			std::cout << "obj: " << obj.dump().c_str() << "\n";
			EXPECT_TRUE(obj.dump() == "{\"k1\": \"v1\", \"k2\": 42, \"k3\": [\"a\", 123, true, false, null]}");

			EXPECT_TRUE(Json("a").number_value() == 0);
			EXPECT_TRUE(Json("a").string_value() == "a");
			EXPECT_TRUE(Json().number_value() == 0);

			EXPECT_TRUE(obj == json);
			EXPECT_TRUE(Json(42) == Json(42.0));
			EXPECT_TRUE(Json(42) != Json(42.1));

			const string unicode_escape_test =
				R"([ "blah\ud83d\udca9blah\ud83dblah\udca9blah\u0000blah\u1234" ])";

			const char utf8[] = "blah" "\xf0\x9f\x92\xa9" "blah" "\xed\xa0\xbd" "blah"
				"\xed\xb2\xa9" "blah" "\0" "blah" "\xe1\x88\xb4";

			Json uni = Json::parse(unicode_escape_test, err);
			EXPECT_TRUE(uni[0].string_value().size() == (sizeof utf8) - 1);
			EXPECT_TRUE(std::memcmp(uni[0].string_value().data(), utf8, sizeof utf8) == 0);
		}

		TEST(json11, multi_parse)
		{
			const eastl::string good_json = R"( {"k1" : "v1"})";
			const eastl::string bad_json1 = good_json + " {";
			const eastl::string bad_json2 = good_json + R"({"k2":"v2", "k3":[)";
			struct TestMultiParse {
				eastl::string input;
				eastl::string::size_type expect_parser_stop_pos;
				size_t expect_not_empty_elms_count;
				Json expect_parse_res;
			} tests[] = {
				{ " {", 0, 0,{} },
				{ good_json, good_json.size(), 1, Json(eastl::map<string, string>{ { "k1", "v1" } }) },
				{ bad_json1, good_json.size() + 1, 1, Json(eastl::map<string, string>{ { "k1", "v1" } }) },
				{ bad_json2, good_json.size(), 1, Json(eastl::map<string, string>{ { "k1", "v1" } }) },
				{ "{}", 2, 1, Json::object{} },
			};
			for (const auto &tst : tests) {
				eastl::string::size_type parser_stop_pos;
				eastl::string err;
				auto res = Json::parse_multi(tst.input, parser_stop_pos, err);
				EXPECT_TRUE(parser_stop_pos == tst.expect_parser_stop_pos);
				EXPECT_TRUE(
					(size_t)std::count_if(res.begin(), res.end(),
						[](const Json& j) { return !j.is_null(); })
					== tst.expect_not_empty_elms_count);
				if (!res.empty()) {
					EXPECT_TRUE(tst.expect_parse_res == res[0]);
				}
			}
		}

		TEST(json11, object)
		{
			Json my_json = Json::object{
				{ "key1", "value1" },
				{ "key2", false },
				{ "key3", Json::array { 1, 2, 3 } },
			};
			eastl::string json_obj_str = my_json.dump();
			std::cout << "json_obj_str: " << json_obj_str.c_str() << "\n";
			EXPECT_TRUE(json_obj_str == "{\"key1\": \"value1\", \"key2\": false, \"key3\": [1, 2, 3]}");

			class Point {
			public:
				int x;
				int y;
				Point(int x, int y) : x(x), y(y) {}
				Json to_json() const { return Json::array { x, y }; }
			};

			eastl::vector<Point> points = { { 1, 2 },{ 10, 20 },{ 100, 200 } };
			eastl::string points_json = Json(points).dump();
			std::cout << "points_json: " << points_json.c_str() << "\n";
			EXPECT_TRUE(points_json == "[[1, 2], [10, 20], [100, 200]]");
		}
	}
