/* json11
 *
 * json11 is a tiny JSON library for C++11, providing JSON parsing and serialization.
 *
 * The core object provided by the library is json11::Json. A Json object represents any JSON
 * value: null, bool, number (int or double), string (eastl::string), array (eastl::vector), or
 * object (eastl::map).
 *
 * Json objects act like values: they can be assigned, copied, moved, compared for equality or
 * order, etc. There are also helper methods Json::dump, to serialize a Json to a string, and
 * Json::parse (static) to parse a eastl::string as a Json object.
 *
 * Internally, the various types of Json object are represented by the JsonValue class
 * hierarchy.
 *
 * A note on numbers - JSON specifies the syntax of number formatting but not its semantics,
 * so some JSON implementations distinguish between integers and floating-point numbers, while
 * some don't. In json11, we choose the latter. Because some JSON implementations (namely
 * Javascript itself) treat all numbers as the same type, distinguishing the two leads
 * to JSON that will be *silently* changed by a round-trip through those implementations.
 * Dangerous! To avoid that risk, json11 stores all numbers as double internally, but also
 * provides integer helpers.
 *
 * Fortunately, double-precision IEEE754 ('double') can precisely store any integer in the
 * range +/-2^53, which includes every 'int' on most systems. (Timestamps often use int64
 * or long long to avoid the Y2038K problem; a double storing microseconds since some epoch
 * will be exact for +/- 275 years.)
 */

/* Copyright (c) 2013 Dropbox, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/map.h>
#include <memory>
#include <initializer_list>

#include "json11/Module.h"

#ifdef _MSC_VER
    #if _MSC_VER <= 1800 // VS 2013
        #ifndef noexcept
            #define noexcept throw()
        #endif

        #ifndef snprintf
            #define snprintf _snprintf_s
        #endif
    #endif
#endif

namespace json11 {

enum JsonParse {
    STANDARD, COMMENTS
};

class JsonValue;

class Json final {
public:
    // Types
    enum Type {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    // Array and object typedefs
    typedef eastl::vector<Json> array;
    typedef eastl::map<eastl::string, Json> object;

    // Constructors for the various types of JSON value.
	JSON11_API Json() noexcept;                // NUL
	JSON11_API Json(std::nullptr_t) noexcept;  // NUL
	JSON11_API Json(double value);             // NUMBER
    JSON11_API Json(int value);                // NUMBER
    JSON11_API Json(bool value);               // BOOL
    JSON11_API Json(const eastl::string &value); // STRING
    JSON11_API Json(eastl::string &&value);      // STRING
    JSON11_API Json(const char * value);       // STRING
    JSON11_API Json(const array &values);      // ARRAY
    JSON11_API Json(array &&values);           // ARRAY
    JSON11_API Json(const object &values);     // OBJECT
    JSON11_API Json(object &&values);          // OBJECT

    // Implicit constructor: anything with a to_json() function.
    template <class T, class = decltype(&T::to_json)>
    Json(const T & t) : Json(t.to_json()) {}

    // Implicit constructor: map-like objects (eastl::map, eastl::unordered_map, etc)
    template <class M, typename eastl::enable_if<
		eastl::is_constructible<eastl::string, decltype(eastl::declval<M>().begin()->first)>::value
        && eastl::is_constructible<Json, decltype(eastl::declval<M>().begin()->second)>::value,
            int>::type = 0>
    Json(const M & m) : Json(object(m.begin(), m.end())) {}

    // Implicit constructor: vector-like objects (std::list, eastl::vector, std::set, etc)
    template <class V, typename eastl::enable_if<
		eastl::is_constructible<Json, decltype(*eastl::declval<V>().begin())>::value,
            int>::type = 0>
    Json(const V & v) : Json(array(v.begin(), v.end())) {}

    // This prevents Json(some_pointer) from accidentally producing a bool. Use
    // Json(bool(some_pointer)) if that behavior is desired.
	JSON11_API Json(void *) = delete;

    // Accessors
	JSON11_API Type type() const;

    JSON11_API bool is_null()   const { return type() == NUL; }
    JSON11_API bool is_number() const { return type() == NUMBER; }
    JSON11_API bool is_bool()   const { return type() == BOOL; }
    JSON11_API bool is_string() const { return type() == STRING; }
    JSON11_API bool is_array()  const { return type() == ARRAY; }
    JSON11_API bool is_object() const { return type() == OBJECT; }

    // Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
    // distinguish between integer and non-integer numbers - number_value() and int_value()
    // can both be applied to a NUMBER-typed object.
	JSON11_API double number_value() const;
	JSON11_API int int_value() const;

    // Return the enclosed value if this is a boolean, false otherwise.
	JSON11_API bool bool_value() const;
    // Return the enclosed string if this is a string, "" otherwise.
	JSON11_API const eastl::string &string_value() const;
    // Return the enclosed eastl::vector if this is an array, or an empty vector otherwise.
	JSON11_API const array &array_items() const;
    // Return the enclosed eastl::map if this is an object, or an empty map otherwise.
	JSON11_API const object &object_items() const;

    // Return a reference to arr[i] if this is an array, Json() otherwise.
	JSON11_API const Json & operator[](size_t i) const;
    // Return a reference to obj[key] if this is an object, Json() otherwise.
	JSON11_API const Json & operator[](const eastl::string &key) const;

    // Serialize.
	JSON11_API void dump(eastl::string &out) const;
	JSON11_API eastl::string dump() const {
        eastl::string out;
        dump(out);
        return out;
    }

    // Parse. If parse fails, return Json() and assign an error message to err.
	JSON11_API static Json parse(const eastl::string & in,
                      eastl::string & err,
                      JsonParse strategy = JsonParse::STANDARD);
	JSON11_API static Json parse(const char * in,
                      eastl::string & err,
                      JsonParse strategy = JsonParse::STANDARD) {
        if (in) {
            return parse(eastl::string(in), err, strategy);
        } else {
            err = "null input";
            return nullptr;
        }
    }
    // Parse multiple objects, concatenated or separated by whitespace
	JSON11_API static eastl::vector<Json> parse_multi(
        const eastl::string & in,
        eastl::string::size_type & parser_stop_pos,
        eastl::string & err,
        JsonParse strategy = JsonParse::STANDARD);

	JSON11_API static inline eastl::vector<Json> parse_multi(
        const eastl::string & in,
        eastl::string & err,
        JsonParse strategy = JsonParse::STANDARD) {
        eastl::string::size_type parser_stop_pos;
        return parse_multi(in, parser_stop_pos, err, strategy);
    }

    JSON11_API bool operator== (const Json &rhs) const;
    JSON11_API bool operator<  (const Json &rhs) const;
    JSON11_API bool operator!= (const Json &rhs) const { return !(*this == rhs); }
    JSON11_API bool operator<= (const Json &rhs) const { return !(rhs < *this); }
    JSON11_API bool operator>  (const Json &rhs) const { return  (rhs < *this); }
    JSON11_API bool operator>= (const Json &rhs) const { return !(*this < rhs); }

    /* has_shape(types, err)
     *
     * Return true if this is a JSON object and, for each item in types, has a field of
     * the given type. If not, return false and set err to a descriptive message.
     */
    typedef std::initializer_list<std::pair<eastl::string, Type>> shape;
	JSON11_API bool has_shape(const shape & types, eastl::string & err) const;

private:
    std::shared_ptr<JsonValue> m_ptr;
};

// Internal class hierarchy - JsonValue objects are not exposed to users of this API.
class JsonValue {
protected:
    friend class Json;
    friend class JsonInt;
    friend class JsonDouble;
    virtual Json::Type type() const = 0;
    virtual bool equals(const JsonValue * other) const = 0;
    virtual bool less(const JsonValue * other) const = 0;
    virtual void dump(eastl::string &out) const = 0;
    virtual double number_value() const;
    virtual int int_value() const;
    virtual bool bool_value() const;
    virtual const eastl::string &string_value() const;
    virtual const Json::array &array_items() const;
    virtual const Json &operator[](size_t i) const;
    virtual const Json::object &object_items() const;
    virtual const Json &operator[](const eastl::string &key) const;
    virtual ~JsonValue() {}
};

} // namespace json11
