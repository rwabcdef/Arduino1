/*
 * Transport_config.hpp
 *
 *  Created on: 18 Dec 2023
 *      Author: rw123
 */

#ifndef TRANSPORT_CONFIG_HPP_
#define TRANSPORT_CONFIG_HPP_


// If defined, then sockets are enabled (i.e. they will be used by the transport layer)
#define TRANSPORT_CONFIG__SOCKETS_ENABLED 1

#if defined(TRANSPORT_CONFIG__SOCKETS_ENABLED)

#define TRANSPORT_CONFIG__NUM_SOCKETS_MAX 5

#endif

#endif /* TRANSPORT_CONFIG_HPP_ */
