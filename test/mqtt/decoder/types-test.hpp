/*
 * FlightVars
 * Copyright (c) 2014 Alvaro Polo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/test/unit_test.hpp>

#include <flightvars/io/buffer.hpp>
#include <flightvars/mqtt/decoder/types.hpp>

using namespace flightvars::mqtt::decoder;

BOOST_AUTO_TEST_SUITE(MqttDecoderString)

BOOST_AUTO_TEST_CASE(MustDecodeString) {
    auto buff = buffer({ 0x00, 0x03, 0x41, 0x42, 0x43 });
    auto abc = decoder<std::string>::decode(buff);
    BOOST_CHECK_EQUAL("ABC", abc);
}

BOOST_AUTO_TEST_CASE(MustDecodeLargeString) {
    buffer buff;
    buff.safe_write_value<std::uint16_t>(to_big_endian<std::uint16_t>(1000));
    for (int i = 0; i < 1000; i++) {
        buff.safe_write_value(char((i % 26) + 0x41));
    }
    buff.flip();
    auto abc = decoder<std::string>::decode(buff);
    BOOST_CHECK_EQUAL(1000, abc.size());
    BOOST_CHECK_EQUAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ", abc.substr(0, 26));
}

BOOST_AUTO_TEST_SUITE_END()