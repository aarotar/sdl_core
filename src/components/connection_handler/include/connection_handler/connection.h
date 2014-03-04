/**
 * \file Connection.hpp
 * \brief Connection class.
 * Stores connection information
 *
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SRC_COMPONENTS_CONNECTION_HANDLER_INCLUDE_CONNECTION_HANDLER_CONNECTION_H_
#define SRC_COMPONENTS_CONNECTION_HANDLER_INCLUDE_CONNECTION_HANDLER_CONNECTION_H_

#include <map>
#include <vector>

#include "utils/logger.h"
#include "utils/lock.h"
#include "connection_handler/device.h"
#include "connection_handler/heartbeat_monitor.h"
#include "protocol_handler/service_type.h"
#include "security_manager/ssl_context.h"

/**
 * \namespace connection_handler
 * \brief SmartDeviceLink connection_handler namespace.
 */
namespace connection_handler {

class ConnectionHandler;

/**
 * \brief Type for ConnectionHandle
 */
typedef int32_t ConnectionHandle;

/**
 * \brief Type for Connections map
 * Key is ConnectionHandle which is unique
 */
typedef std::map<int32_t, Connection*> ConnectionList;

/**
 * \brief Type for Connections map iterator
 */
typedef ConnectionList::iterator ConnectionListIterator;

struct Service {
  protocol_handler::ServiceType service_type;
  security_manager::SSLContext* ssl_context;
  Service()
    : service_type(protocol_handler::kInvalidServiceType),
      ssl_context(NULL) {
  }
  Service(protocol_handler::ServiceType service_type)
    : service_type(service_type),
      ssl_context(NULL) {
  }
  bool operator==(const protocol_handler::ServiceType service_type) const {
    return this->service_type == service_type;
  }
};

/**
 * \brief Type for Session Services
 */
typedef std::vector<Service> ServiceList;

/**
 * \brief Type for Services iterator
 */
typedef ServiceList::iterator ServiceListIterator;

/**
 * \brief Type for Services iterator
 */
typedef ServiceList::const_iterator ServiceListConstIterator;

/**
 * \brief Type for Services iterator
 */
typedef std::map<uint8_t, ServiceList> SessionMap;

/**
 * \brief Type for Services iterator
 */
typedef SessionMap::iterator SessionMapIterator;

/**
 * \brief Type for Services const iterator
 */
typedef SessionMap::const_iterator SessionMapConstIterator;

/**
 * \class Connection
 * \brief Connection class
 */
class Connection {
 public:
  /**
   * \brief Class constructor
   */
  Connection(ConnectionHandle connection_handle,
             DeviceHandle connection_device_handle,
             ConnectionHandler* connection_handler,
             int32_t heartbeat_timeout);

  /**
   * \brief Destructor
   */
  ~Connection();

  /**
   * \brief Returns device handle
   * \return DeviceHandle
   */
  ConnectionHandle connection_handle() const;

  /**
   * \brief Returns connection device handle
   * \return ConnectionDeviceHandle
   */
  DeviceHandle connection_device_handle();

  /**
   * \brief Adds session to connection
   * \return sessionID or -1 in case of issues
   */
  int32_t AddNewSession();

  /**
   * \brief Removes session from connection
   * \param session session ID
   * \return sessionID or -1 in case of issues
   */
  int32_t RemoveSession(uint8_t session);

  /**
   * \brief Adds service to session
   * \return TRUE on success, otherwise FALSE
   */
  bool AddNewService(uint8_t session,
                     protocol_handler::ServiceType service);

  /**
   * \brief Removes service from session
   * \param session session ID
   * \return TRUE on success, otherwise FALSE
   */
  bool RemoveService(uint8_t session,
                     protocol_handler::ServiceType service_type);

  /**
   * \brief Sets crypto context of service
     * \param sessionId Identifier of the session
   * \param service_type Type of service
   * \return \c true in case of service is protected or \c false otherwise
   */
  bool SetSSLContext(
    uint8_t session,
    protocol_handler::ServiceType service_type,
    security_manager::SSLContext* context);

  /**
   * \brief Gets crypto context of service
     * \param sessionId Identifier of the session
   * \param service_type Type of service
   * \return \c true in case of service is protected or \c false otherwise
   */
  security_manager::SSLContext* GetSSLContext(
      uint8_t session,
      protocol_handler::ServiceType service_type) const;
  /**
   * \brief Returns map of sessions which have been opened in
   *  current connection.
   */
  const SessionMap session_map() const;

  /*
   * \brief Close this connection and all associated sessions
   */
  void Close();

  /*
   * \brief Prevent this connection from being closed by heartbeat timeout
   */
  void KeepAlive();

 private:
  ConnectionHandler* connection_handler_;

  /**
   * \brief Current connection handle.
   */
  ConnectionHandle connection_handle_;

  /**
   * \brief DeviceHandle of this connection.
   */
  DeviceHandle connection_device_handle_;

  /**
   * \brief session/services map
   */
  SessionMap  session_map_;

  mutable sync_primitives::Lock session_map_lock_;

  /*
   * \brief monitor that closes connection if there is no traffic over it
   */
  HeartBeatMonitor heartbeat_monitor_;

  /**
   * \brief For logging.
   */
  static log4cxx::LoggerPtr logger_;
};

}/* namespace connection_handler */

#endif  // SRC_COMPONENTS_CONNECTION_HANDLER_INCLUDE_CONNECTION_HANDLER_CONNECTION_H_
