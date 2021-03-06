/**
 * @file   pools-conn.hpp
 * @author Sergey Bryukov <sbryukov@gmail.com>
 *
 * @brief
 *
 *
 */

#ifndef _POOLS_CONN_H
#define _POOLS_CONN_H 1

#include "common.h"
#include <boost/unordered_map.hpp>
#include <boost/lexical_cast.hpp>
#include <jsoncpp/json/json.h>

class connection : public boost::enable_shared_from_this<connection> {
public:
	typedef boost::shared_ptr<connection> pointer;

	static pointer create(ba::io_service& io_service) {
		return pointer(new connection(io_service));
	}

	ba::ip::tcp::socket& socket() {
		return bsocket_;
	}

	/// Start read data of request from browser
	void start();

private:
	connection(ba::io_service& io_service);

	/// Read json from mainer
	void handle_miner_read_headers(const bs::error_code& err, size_t len);
	
	/// Start connecting to the web-server, initially to resolve the DNS-name of Web server into the IP address
	void start_connect_bitcoind();
	void handle_resolve_bitcoind(const boost::system::error_code& err,
									ba::ip::tcp::resolver::iterator endpoint_iterator);
	void handle_connect_bitcoind(const boost::system::error_code& err,
									ba::ip::tcp::resolver::iterator endpoint_iterator, const bool first_time);

	/// Write data to/from the bitcoind
	void start_write_to_bitcoind();
	void handle_bitcoind_server_write(const bs::error_code& err, size_t len);
	void handle_bitcoind_read_headers(const bs::error_code& err, size_t len);

	/// Reading data from a bitcoind server, and writing it to the mainer
	void handle_miner_write(const bs::error_code& err, size_t len);
	void handle_server_read_body(const bs::error_code& err, size_t len);

	/// Close both sockets: for browser and web-server
	void shutdown();



	ba::io_service& io_service_;
	ba::ip::tcp::socket bsocket_;
	ba::ip::tcp::socket ssocket_;
	ba::ip::tcp::resolver resolver_;
	bool proxy_closed;
	bool isPersistent;
	int32_t RespLen;
	int32_t RespReaded;

	boost::array<char, 8192> bbuffer;
	boost::array<char, 8192> sbuffer;

	std::string fURL;
	std::string miner_request_;
	std::string fNewURL;
	std::string fMethod;
	std::string fReqVersion;
	std::string fServer_;
	std::string fPort_;
	bool isOpened;

	unsigned mmainer_message_id_;

	std::string fReq;

	typedef boost::unordered_map<std::string,std::string> headersMap;
	headersMap reqHeaders, respHeaders;

	void parse_json_from_miner(const std::string& h, headersMap& hm);
};


#endif /* _POOLS-CONN_H */

