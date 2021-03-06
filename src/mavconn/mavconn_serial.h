/**
 * @file mavconn_serial.h
 * @author Vladimir Ermakov <vooon341@gmail.com>
 */
/*
 * Copyright 2013 Vladimir Ermakov.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#pragma once

#include <mavros/mavconn_interface.h>
#include <boost/asio/serial_port.hpp>
#include <boost/shared_array.hpp>

namespace mavconn {

class MAVConnSerial : public MAVConnInterface {
public:
	MAVConnSerial(uint8_t system_id = 1, uint8_t component_id = MAV_COMP_ID_UDP_BRIDGE,
			std::string device = "/dev/ttyACM0", unsigned baudrate = 57600);
	~MAVConnSerial();

	using MAVConnInterface::send_message;
	void send_message(const mavlink_message_t *message, uint8_t sysid, uint8_t compid);
	void send_bytes(const uint8_t *bytes, size_t length);

	inline mavlink_status_t get_status() { return *mavlink_get_channel_status(channel); };
	inline bool is_open() { return serial_dev.is_open(); };

private:
	asio::io_service io_service;
	boost::thread io_thread;
	asio::serial_port serial_dev;

	static const size_t RX_BUFSIZE = MAVLINK_MAX_PACKET_LEN;
	uint8_t rx_buf[RX_BUFSIZE];
	std::vector<uint8_t> tx_q;
	boost::shared_array<uint8_t> tx_buf;
	size_t tx_buf_size;
	boost::recursive_mutex mutex;

	void do_read(void);
	void async_read_end(boost::system::error_code ec, size_t bytes_transfered);
	void do_write(void);
	void async_write_end(boost::system::error_code ec);
};

}; // namespace mavconn

