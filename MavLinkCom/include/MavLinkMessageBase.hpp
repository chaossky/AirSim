// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef MavLinkCom_MavLinkMessageBase_hpp
#define MavLinkCom_MavLinkMessageBase_hpp

#include <stdint.h>
#include <string>
#include <sstream>
#include <memory>
namespace mavlinkcom_impl {
	class MavLinkConnectionImpl;
	class MavLinkNodeImpl;
}
namespace mavlinkcom
{
	class MavLinkConnection;

#define PayloadSize ((255 + 2 + 7) / 8)

	// This is the raw undecoded message, use the msgid field to figure out which strongly typed
	// MavLinkMessageBase subclass can be used to decode this message.  For example, a heartbeat:
	// MavLinkMessage msg;
	// if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
	//     MavLinkHeartbeat heartbeat;
	//     heartbeat.decode(msg);
	//     ...
	// }
	class MavLinkMessage {
	public:
        uint16_t checksum;      ///< sent at end of packet
        uint8_t magic;          ///< protocol magic marker
        uint8_t len;            ///< Length of payload
        uint8_t incompat_flags; ///< flags that must be understood
        uint8_t compat_flags;   ///< flags that can be ignored if not understood
        uint8_t seq;            ///< Sequence of packet
        uint8_t sysid;          ///< ID of message sender system/aircraft
        uint8_t compid;         ///< ID of the message sender component
        uint32_t msgid : 24;      ///< ID of message in payload
        uint64_t payload64[PayloadSize];
        uint8_t ck[2];          ///< incoming checksum bytes
        uint8_t signature[13];
	};

	// This is the base class for all the strongly typed messages define in MavLinkMessages.hpp
	class MavLinkMessageBase
	{
	public:
		uint8_t msgid = 0;
		uint8_t sysid = 0;   ///< ID of message sender system/aircraft
		uint8_t compid = 0;  ///< ID of the message sender component
        uint64_t timestamp = 0;

		// unpack the given message
		void decode(const MavLinkMessage& msg);
        // pack this message into given message buffer 
        void encode(MavLinkMessage& msg) const;

		// find what type of message this is and decode it on the heap (call delete when you are done with it).
		static MavLinkMessageBase* lookup(const MavLinkMessage& msg);
		virtual std::string toJSon() = 0;
        virtual ~MavLinkMessageBase() {}
	protected:
		virtual int pack(char* buffer) const = 0;
		virtual int unpack(const char* buffer) = 0;

		void pack_uint8_t(char* buffer, const uint8_t* field, int offset) const;
		void pack_int8_t(char* buffer, const int8_t* field, int offset) const;
		void pack_int16_t(char* buffer, const int16_t* field, int offset) const;
		void pack_uint16_t(char* buffer, const uint16_t* field, int offset) const;
		void pack_uint32_t(char* buffer, const uint32_t* field, int offset) const;
		void pack_int32_t(char* buffer, const int32_t* field, int offset) const;
		void pack_uint64_t(char* buffer, const uint64_t* field, int offset) const;
		void pack_int64_t(char* buffer, const int64_t* field, int offset) const;
		void pack_float(char* buffer, const float* field, int offset) const;
		void pack_char_array(int len, char* buffer, const char* field, int offset) const;
		void pack_uint8_t_array(int len, char* buffer, const uint8_t* field, int offset) const;
		void pack_int8_t_array(int len, char* buffer, const int8_t* field, int offset) const;
		void pack_uint16_t_array(int len, char* buffer, const uint16_t* field, int offset) const;
		void pack_int16_t_array(int len, char* buffer, const int16_t* field, int offset) const;
		void pack_float_array(int len, char* buffer, const float* field, int offset) const;
		
		void unpack_uint8_t(const char* buffer, uint8_t* field, int offset);
		void unpack_int8_t(const char* buffer, int8_t* field, int offset);
		void unpack_int16_t(const char* buffer, int16_t* field, int offset);
		void unpack_uint16_t(const char* buffer, uint16_t* field, int offset);
		void unpack_uint32_t(const char* buffer, uint32_t* field, int offset);
		void unpack_int32_t(const char* buffer, int32_t* field, int offset);
		void unpack_uint64_t(const char* buffer, uint64_t* field, int offset);
		void unpack_int64_t(const char* buffer, int64_t* field, int offset);
		void unpack_float(const char* buffer, float* field, int offset);
		void unpack_char_array(int len, const char* buffer, char* field, int offset);
		void unpack_uint8_t_array(int len, const char* buffer, uint8_t* field, int offset);
		void unpack_int8_t_array(int len, const char* buffer, int8_t* field, int offset);
		void unpack_uint16_t_array(int len, const char* buffer, uint16_t* field, int offset);
		void unpack_int16_t_array(int len, const char* buffer, int16_t* field, int offset);
		void unpack_float_array(int len, const char* buffer, float* field, int offset);

		std::string char_array_tostring(int len, const char* field);
		std::string uint8_t_array_tostring(int len, const uint8_t* field);
		std::string int8_t_array_tostring(int len, const int8_t* field);
		std::string int16_t_array_tostring(int len, const int16_t* field);
		std::string uint16_t_array_tostring(int len, const uint16_t* field);
		std::string float_array_tostring(int len, const float* field);
		std::string float_tostring(float value); 
	};

	// Base class for all strongly typed MavLinkCommand classes defined in MavLinkMessages.hpp
	class MavLinkCommand {
	public:
		uint16_t command = 0;
	protected:
		virtual void pack() = 0;
		virtual void unpack() = 0;

		float param1 = 0;
		float param2 = 0;
		float param3 = 0;
		float param4 = 0;
		float param5 = 0;
		float param6 = 0;
		float param7 = 0;

		friend class mavlinkcom_impl::MavLinkNodeImpl;
        friend class mavlinkcom_impl::MavLinkConnectionImpl;
	};


	// The location of a landing area captured from a downward facing camera
	class MavLinkTelemetry : public MavLinkMessageBase {
	public:
		const static uint8_t kMessageId = 204; // in the user range 180-229.
		MavLinkTelemetry() { msgid = kMessageId; }
		uint32_t messagesSent;		 // number of messages sent since the last telemetry message
		uint32_t messagesReceived;	 // number of messages received since the last telemetry message
		uint32_t messagesHandled;	 // number of messages handled since the last telemetry message
		uint32_t crcErrors;			 // # crc errors detected in mavlink stream since the last telemetry message
		uint32_t handlerMicroseconds; // total time spent in the handlers in microseconds since the last telemetry message
		uint32_t renderTime;         // total time spent rendering frames since the last telemetry message
        const char* wifiInterfaceName; // the name of the wifi interface we are measuring RSSI on.
        int32_t wifiRssi;            // if this device is communicating over wifi this is the signal strength.
		virtual std::string toJSon() {

			std::ostringstream result;
			result << "\"MavLinkTelemetry\"" << " : { ";
			result << "\"messagesSent\":" << this->messagesSent << ",";
			result << "\"messagesReceived\":" << this->messagesReceived << ",";
			result << "\"messagesHandled\":" << this->messagesHandled << ",";
			result << "\"crcErrors\":" << this->crcErrors << ",";
			result << "\"handlerMicroseconds\":" << this->handlerMicroseconds << ",";
			result << "\"renderTime\":" << this->renderTime;
            result << "\"wifiRssi\":" << this->wifiRssi;
			result << "}";
			return result.str();
		}
	protected:
		virtual int pack(char* buffer) const;
		virtual int unpack(const char* buffer);
	};

}

#endif
