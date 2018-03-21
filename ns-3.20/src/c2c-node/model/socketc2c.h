/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#ifndef __SOCKET_C2C_H__
#define __SOCKET_C2C_H__

#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/tag.h"
#include "ns3/object.h"
#include "ns3/c2c-address.h"
#include <stdint.h>

namespace ns3 {


class Node;
class Packet;

/**
 * \ingroup node
 * \defgroup socketc2c Socketc2c
 */

/**
 * \brief  iTETRIS [WP600: HITA] - A low-level socket API based loosely on the BSD socket API.
 * \ingroup socketc2c
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class Socketc2c : public Object
{
public:

  Socketc2c (void);
  virtual ~Socketc2c (void);

//    TODO Socketc2c Error Logging
  enum SocketErrno {
    ERROR_NOTERROR,
    ERROR_ISCONN,
    ERROR_NOTCONN,
    ERROR_MSGSIZE,
    ERROR_AGAIN,
    ERROR_SHUTDOWN,
    ERROR_OPNOTSUPP,
    ERROR_AFNOSUPPORT,
    ERROR_INVAL,
    ERROR_BADF,
    ERROR_NOROUTETOHOST,
    socketc2c_ERRNO_LAST
  };

  /**
   * This method wraps the creation of c2c Sockets that is performed
   * by a Socketc2c factory on a given node based on a TypeId.
   *
   * \return A smart pointer to a newly created Socketc2c.
   *
   * \param node The node on which to create the c2c Socket
   * \param tid The TypeId of the c2c Socket to create
   */
  static Ptr<Socketc2c> CreateSocket (Ptr<Node> node, TypeId tid);
  /**
   * \returns the node this c2c Socket is associated with.
   */
  virtual Ptr<Node> GetNode (void) const = 0;

  /**
   * \brief Notify application when new data is available to be read.
   *
   *        This callback is intended to notify a c2c Socket that would
   *        have been blocked in a blocking Socket model that data
   *        is available to be read.
   */
  void SetRecvCallback (Callback<void, Ptr<Socketc2c> >);

  /**
   * \param port the port to try to allocate
   * \returns 0 on success, -1 on failure.
   *
   * Allocate a local endpoint for this Socketc2c.
   */
  virtual int Bind (const uint16_t port) = 0;

  /**
   * \brief Close a Socketc2c.
   * \returns zero on success, -1 on failure.
   *
   * After the Close call, the Socketc2c is no longer valid, and cannot
   * safely be used for subsequent operations.
   */
  virtual int Close (void) = 0;

  /**
   * \brief Returns the number of bytes which can be sent in a single call
   * to Send.
   *
   * For datagram Sockets, this returns the number of bytes that
   * can be passed atomically through the underlying protocol.
   *
   * For stream Sockets, this returns the available space in bytes
   * left in the transmit buffer.
*/
  virtual uint32_t GetTxAvailable (void) const = 0;

  /**
   * \brief Send data (or dummy data) to the remote host
   *
   * This function matches closely in semantics to the send() function
   * call in the standard C library (libc):
   *   ssize_t send (int s, const void *msg, size_t len, int flags);
   * except that the send I/O is asynchronous.  This is the
   * primary Send method at this low-level API and must be implemented
   * by subclasses.
   *
   * In a typical blocking Sockets model, this call would block upon
   * lack of space to hold the message to be sent.  In ns-3 at this
   * API, the call returns immediately in such a case, but the callback
   * registered with SetSendCallback() is invoked when the Socketc2c
   * has space (when it conceptually unblocks); this is an asynchronous
   * I/O model for send().
   *
   * This variant of Send() uses class ns3::Packet to encapsulate
   * data, rather than providing a raw pointer and length field.
   * This allows an ns-3 application to attach tags if desired (such
   * as a flow ID) and may allow the simulator to avoid some data
   * copies.  Despite the appearance of sending Packets on a stream
   * Socketc2c, just think of it as a fancy byte buffer with streaming
   * semantics.
   *
   * If either the message buffer within the Packet is too long to pass
   * atomically through the underlying protocol (for datagram Socketc2cs),
   * or the message buffer cannot entirely fit in the transmit buffer
   * (for stream Socketc2cs), -1 is returned and Socketc2cErrno is set
   * to ERROR_MSGSIZE.  If the packet does not fit, the caller can
   * split the Packet (based on information obtained from
   * GetTxAvailable) and re-attempt to send the data.
   *
   * The flags argument is formed by or'ing one or more of the values:
   *        MSG_OOB        process out-of-band data
   *        MSG_DONTROUTE  bypass routing, use direct interface
   * These flags are _unsupported_ as of ns-3.1.
   *
   * \param p ns3::Packet to send
   * \param flags Socketc2c control flags
   * \returns the number of bytes accepted for transmission if no error
   *          occurs, and -1 otherwise.
   *
   * \see SetSendCallback
*/
  virtual int Send (Ptr<Packet> p, uint32_t flags) = 0;

  /**
   * Return number of bytes which can be returned from one or
   * multiple calls to Recv.
   * Must be possible to call this method from the Recv callback.
   */
  virtual uint32_t GetRxAvailable (void) const = 0;

  /**
   * \brief Read data from the Socketc2c
   *
   * This function matches closely in semantics to the recv() function
   * call in the standard C library (libc):
   *   ssize_t recv (int s, void *buf, size_t len, int flags);
   * except that the receive I/O is asynchronous.  This is the
   * primary Recv method at this low-level API and must be implemented
   * by subclasses.
   *
   * This method is normally used only on a connected Socket.
   * In a typical blocking Sockets model, this call would block until
   * at least one byte is returned or the connection closes.
   * In ns-3 at this API, the call returns immediately in such a case
   * and returns 0 if nothing is available to be read.
   * However, an application can set a callback, ns3::SetRecvCallback,
   * to be notified of data being available to be read
   * (when it conceptually unblocks); this is an asynchronous
   * I/O model for recv().
   *
   * This variant of Recv() uses class ns3::Packet to encapsulate
   * data, rather than providing a raw pointer and length field.
   * This allows an ns-3 application to attach tags if desired (such
   * as a flow ID) and may allow the simulator to avoid some data
   * copies.  Despite the appearance of receiving Packets on a stream
   * Socketc2c, just think of it as a fancy byte buffer with streaming
   * semantics.
   *
   * The semantics depend on the type of Socketc2c.  For a datagram Socketc2c,
   * each Recv() returns the data from at most one Send(), and order
   * is not necessarily preserved.  For a stream Socketc2c, the bytes
   * are delivered in order, and on-the-wire packet boundaries are
   * not preserved.
   *
   * The flags argument is formed by or'ing one or more of the values:
   *        MSG_OOB             process out-of-band data
   *        MSG_PEEK            peek at incoming message
   * None of these flags are supported for now.
   *
   * Some variants of Recv() are supported as additional API,
   * including RecvFrom(), overloaded Recv() without arguments,
   * and variants that use raw character buffers.
   *
   * \param maxSize reader will accept packet up to maxSize
   * \param flags Socketc2c control flags
   * \returns Ptr<Packet> of the next in-sequence packet.  Returns
   * 0 if the Socketc2c cannot return a next in-sequence packet conforming
   * to the maxSize and flags.
   *
   * \see SetRecvCallback
*/
  virtual Ptr<Packet> Recv (uint32_t maxSize, uint32_t flags) = 0;

/*
  *
   * \brief Read a single packet from the Socketc2c and retrieve the sender
   * address.
   *
   * Calls Recv(maxSize, flags) with maxSize
   * implicitly set to maximum sized integer, and flags set to zero.
   *
   * This method has similar semantics to Recv () but subclasses may
   * want to provide checks on Socketc2c state, so the implementation is
   * pushed to subclasses.
   *
   * \param maxSize reader will accept packet up to maxSize
   * \param flags Socketc2c control flags
   * \param fromAddress output parameter that will return the
   * address of the sender of the received packet, if any.  Remains
   * untouched if no packet is received.
   * \returns Ptr<Packet> of the next in-sequence packet.  Returns
   * 0 if the Socketc2c cannot return a next in-sequence packet.
*/
  virtual Ptr<Packet> RecvFrom (uint32_t maxSize, uint32_t flags,
		  const uint64_t fromNodeId) = 0;

  /**
  /////////////////////////////////////////////////////////////////////
  //   The remainder of these public methods are overloaded methods  //
  //   or variants of Send() and Recv(), and they are non-virtual    //
  /////////////////////////////////////////////////////////////////////

   *
   * \brief Send data (or dummy data) to the remote host
   *
   * Overloaded version of Send(..., flags) with flags set to zero.
   *
   * \param p ns3::Packet to send
   * \returns the number of bytes accepted for transmission if no error
   *          occurs, and -1 otherwise.

   */
   int Send (Ptr<Packet> p);

  /** \brief Read a single packet from the Socketc2c
   *
   * Overloaded version of Recv(maxSize, flags) with maxSize
   * implicitly set to maximum sized integer, and flags set to zero.
   *
   * \returns Ptr<Packet> of the next in-sequence packet.  Returns
   * 0 if the Socketc2c cannot return a next in-sequence packet.
   */
   virtual Ptr<Packet> Recv (void) = 0;

  /** \brief send a packet to a specific c2c address
   *
   */
  virtual int DoSendTo (Ptr<Packet> p, Ptr <c2cAddress> daddr, uint16_t dport, uint8_t lt, uint8_t tc, uint16_t sn)=0;

  /**
   * \brief Recv data (or dummy data) from the remote host
   *
   * This method is provided so as to have an API which is closer in
   * appearance to that of real network or BSD Socketc2cs.
   *
   * If the underlying packet was carring null (fake) data, this buffer
   * will be zeroed up to the length specified by the return value.
   *
   * \param buf A pointer to a raw byte buffer to write the data to.
   * \param size Number of bytes (at most) to copy to buf
   * \param flags any flags to pass to the Socketc2c
   * \returns number of bytes copied into buf
   */
  int Recv (uint8_t* buf, uint32_t size, uint32_t flags);

  /**
   * \brief Read a single packet from the Socketc2c and retrieve the sender
   * address.
   *
   * Calls RecvFrom (maxSize, flags, fromNodeId) with maxSize
   * implicitly set to maximum sized integer, and flags set to zero.
   *
   * \param fromAddress output parameter that will return the
   * address of the sender of the received packet, if any.  Remains
   * untouched if no packet is received.
   * \returns Ptr<Packet> of the next in-sequence packet.  Returns
   * 0 if the Socketc2c cannot return a next in-sequence packet.
   */
  Ptr<Packet> RecvFrom (uint64_t fromNodeId);

  /**
   * \brief Read a single packet from the Socketc2c and retrieve the sender
   * address.
   *
   * This method is provided so as to have an API which is closer in
   * appearance to that of real network or BSD Socketc2cs.
   *
   * \param buf A pointer to a raw byte buffer to write the data to.
   * If the underlying packet was carring null (fake) data, this buffer
   * will be zeroed up to the length specified by the return value.
   * \param size Number of bytes (at most) to copy to buf
   * \param flags any flags to pass to the Socketc2c
   * \param fromAddress output parameter that will return the
   * address of the sender of the received packet, if any.  Remains
   * untouched if no packet is received.
   * \returns number of bytes copied into buf
   */
  int RecvFrom (uint8_t* buf, uint32_t size, uint32_t flags,
		       uint64_t fromNodeId);



protected:
  void NotifyDataSent (uint32_t size);
  void NotifySend (uint32_t spaceAvailable);
  void NotifyDataRecv (void);
  virtual void DoDispose (void);

private:
  Callback<void, Ptr<Socketc2c>, uint32_t>          m_dataSent;
  Callback<void, Ptr<Socketc2c>, uint32_t >         m_sendCb;
  Callback<void, Ptr<Socketc2c> >                   m_receivedData;

};

} //namespace ns3

#endif /* Socketc2c_H */


