/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 The Boeing Company
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
 * Authors: Gary Pei <guangyu.pei@boeing.com>
 *          Sébastien Deronne <sebastien.deronne@gmail.com>
 */

#include <cmath>
#include <math.h> //Baldo log10
#include "DSRC-error-rate-model.h"
#include "wifi-phy.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DSRCErrorRateModel");

NS_OBJECT_ENSURE_REGISTERED (DSRCErrorRateModel);

TypeId
DSRCErrorRateModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DSRCErrorRateModel")
    .SetParent<ErrorRateModel> ()
    //.SetGroupName ("Wifi")
    .AddConstructor<DSRCErrorRateModel> ()
  ;
  return tid;
}

DSRCErrorRateModel::DSRCErrorRateModel ()
{
}

double
DSRCErrorRateModel::GetBpskBer (double snr) const
{
  NS_LOG_FUNCTION (this << snr);
  double z = std::sqrt (snr);
  double ber = 0.5 * erfc (z);
  NS_LOG_INFO ("bpsk snr=" << snr << " ber=" << ber);
  return ber;
}

double
DSRCErrorRateModel::GetQpskBer (double snr) const
{
  NS_LOG_FUNCTION (this << snr);
  double z = std::sqrt (snr / 2.0);
  double ber = 0.5 * erfc (z);
  NS_LOG_INFO ("qpsk snr=" << snr << " ber=" << ber);
  return ber;
}

double
DSRCErrorRateModel::Get16QamBer (double snr) const
{
  NS_LOG_FUNCTION (this << snr);
  double z = std::sqrt (snr / (5.0 * 2.0));
  double ber = 0.75 * 0.5 * erfc (z);
  NS_LOG_INFO ("16-Qam" << " snr=" << snr << " ber=" << ber);
  return ber;
}

double
DSRCErrorRateModel::Get64QamBer (double snr) const
{
  NS_LOG_FUNCTION (this << snr);
  double z = std::sqrt (snr / (21.0 * 2.0));
  double ber = 7.0 / 12.0 * 0.5 * erfc (z);
  NS_LOG_INFO ("64-Qam" << " snr=" << snr << " ber=" << ber);
  return ber;
}
double
DSRCErrorRateModel::Get256QamBer (double snr) const
{
  NS_LOG_FUNCTION (this << snr);
  double z = std::sqrt (snr / (85.0 * 2.0));
  double ber = 15.0 / 32.0 * 0.5 * erfc (z);
  NS_LOG_INFO ("256-Qam" << " snr=" << snr << " ber=" << ber);
  return ber;
}

double
DSRCErrorRateModel::GetFecBpskBer (double snr, uint32_t nbits,
                                   uint32_t bValue) const
{
  NS_LOG_FUNCTION (this << snr << nbits << bValue);
  double ber = GetBpskBer (snr);
  if (ber == 0.0)
    {
      return 1.0;
    }
  double pe = CalculatePe (ber, bValue);
  pe = std::min (pe, 1.0);
  double pms = std::pow (1 - pe, (double)nbits);
  return pms;
}
                                                                        //Estos valores NO están en la curva
double EbNo_dB_table [8] = { 0,     5,      10,     15,     20,         25,         30,         35 }; 
double ber_L1        [8] = { 0.5,   0.45,   0.12,   0.0007, 0.00025,    0.00015,    0.00009,    0.00007 }; 

double 
DSRCErrorRateModel::GetBERQpsk_IEEE80211p(double snr) const
{
    /**This is to be used when the the PHY layer is configure with
     * phyModeDSRC = "OfdmRate6MbpsBW10MHz"
     * YansWifiPhyHelper DSRCPhy =  YansWifiPhyHelper::Default ();
     * DSRCPhy.SetErrorRateModel ("ns3::DSRCErrorRateModel");
     * Wifi80211pHelper DSRC = Wifi80211pHelper::Default ();
     * DSRC.SetStandard(WIFI_PHY_STANDARD_80211_10MHZ);
     * DSRC.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
     *                               "DataMode",StringValue (phyModeDSRC),
     *                               "ControlMode",StringValue (phyModeDSRC),
     *                               "NonUnicastMode", StringValue (phyModeDSRC));
     * 
     * Low-Complexity Scalable Iterative Algorithms for IEEE 802.11p Receivers, IEEE TRANSACTIONS ON VEHICULAR TECHNOLOGY, VOL. 64, NO. 9, SEPTEMBER 2015
     */

    
    double BW = 10; //En MHz    
    double bitrateMbps = 6; //En Mbps
    double EbNo_dB_meas = 10*log10(snr) + 10*log10(BW/bitrateMbps); //El segundo termino es porque la grafica es FER vs Eb/No, no SNR    //
    
    //ebno_index marca el punto en abscisas de ebno cuyo valor es inmediatamente mayor    
    int ebno_index = -1;    
    int numPoints = 8;    
    for (int i = 0; i < numPoints; i++) {        
        if (EbNo_dB_meas < EbNo_dB_table[i]) {            
            ebno_index = i;            
            break;        
        } 
        else if (i == (numPoints - 1)) {            
            ebno_index = numPoints;            
            break;        
        }    
    }    
    double ber = -1;    
    if (ebno_index <= 0) {        
        ber = 1;    } 
    else if (ebno_index >= numPoints) {        
        //TODO mínimo BER        
        ber = 0.00001;    
    } else {        
        ber = ber_L1[ebno_index - 1] + (ber_L1[ebno_index] - ber_L1[ebno_index - 1])/(EbNo_dB_table[ebno_index] - EbNo_dB_table[ebno_index - 1])*(EbNo_dB_meas - EbNo_dB_table[ebno_index - 1]);    
    }    
    
    //std::cout << "GetBERQpsk_IEEE80211p: snr=" << snr << ", EbNo_dB_meas=" << EbNo_dB_meas << ", ber=" << ber << std::endl;
    
    //TODO salida    
    /*FILE *fp;     
     * fp = fopen("CurvaFER.csv", "a");     
     * fprintf(fp, "%f, %f\n", EbNo_dB_meas, ber);     
     * fclose(fp);*/    
    return (ber);
}

double
DSRCErrorRateModel::GetFecQpskBer (double snr, uint32_t nbits,
                                   uint32_t bValue) const
{
  NS_LOG_FUNCTION (this << snr << nbits << bValue);
  
  
  /*
   * This is for the BER-EbNo curves of Low-Complexity Scalable Iterative Algorithms for IEEE 802.11p Receivers, IEEE TRANSACTIONS ON VEHICULAR TECHNOLOGY, VOL. 64, NO. 9, SEPTEMBER 2015
   */
  double ber = GetBERQpsk_IEEE80211p(snr);
  
  /*
   * This is the traditional NIST
   *
  double ber = GetQpskBer (snr); */
  
  if (ber == 0.0)
    {
      return 1.0;
    }
  double pe = CalculatePe (ber, bValue);
  pe = std::min (pe, 1.0);
  double pms = std::pow (1 - pe, (double)nbits);
  return pms;
}

double
DSRCErrorRateModel::CalculatePe (double p, uint32_t bValue) const
{
  NS_LOG_FUNCTION (this << p << bValue);
  double D = std::sqrt (4.0 * p * (1.0 - p));
  double pe = 1.0;
  if (bValue == 1)
    {
      //code rate 1/2, use table 3.1.1
      pe = 0.5 * (36.0 * std::pow (D, 10)
                  + 211.0 * std::pow (D, 12)
                  + 1404.0 * std::pow (D, 14)
                  + 11633.0 * std::pow (D, 16)
                  + 77433.0 * std::pow (D, 18)
                  + 502690.0 * std::pow (D, 20)
                  + 3322763.0 * std::pow (D, 22)
                  + 21292910.0 * std::pow (D, 24)
                  + 134365911.0 * std::pow (D, 26));
    }
  else if (bValue == 2)
    {
      //code rate 2/3, use table 3.1.2
      pe = 1.0 / (2.0 * bValue) *
        (3.0 * std::pow (D, 6)
         + 70.0 * std::pow (D, 7)
         + 285.0 * std::pow (D, 8)
         + 1276.0 * std::pow (D, 9)
         + 6160.0 * std::pow (D, 10)
         + 27128.0 * std::pow (D, 11)
         + 117019.0 * std::pow (D, 12)
         + 498860.0 * std::pow (D, 13)
         + 2103891.0 * std::pow (D, 14)
         + 8784123.0 * std::pow (D, 15));
    }
  else if (bValue == 3)
    {
      //code rate 3/4, use table 3.1.2
      pe = 1.0 / (2.0 * bValue) *
        (42.0 * std::pow (D, 5)
         + 201.0 * std::pow (D, 6)
         + 1492.0 * std::pow (D, 7)
         + 10469.0 * std::pow (D, 8)
         + 62935.0 * std::pow (D, 9)
         + 379644.0 * std::pow (D, 10)
         + 2253373.0 * std::pow (D, 11)
         + 13073811.0 * std::pow (D, 12)
         + 75152755.0 * std::pow (D, 13)
         + 428005675.0 * std::pow (D, 14));
    }
  else if (bValue == 5)
    {
      //code rate 5/6, use table V from D. Haccoun and G. Begin, "High-Rate Punctured Convolutional Codes
      //for Viterbi Sequential Decoding", IEEE Transactions on Communications, Vol. 32, Issue 3, pp.315-319.
      pe = 1.0 / (2.0 * bValue) *
        (92.0 * std::pow (D, 4.0)
         + 528.0 * std::pow (D, 5.0)
         + 8694.0 * std::pow (D, 6.0)
         + 79453.0 * std::pow (D, 7.0)
         + 792114.0 * std::pow (D, 8.0)
         + 7375573.0 * std::pow (D, 9.0)
         + 67884974.0 * std::pow (D, 10.0)
         + 610875423.0 * std::pow (D, 11.0)
         + 5427275376.0 * std::pow (D, 12.0)
         + 47664215639.0 * std::pow (D, 13.0));
    }
  else
    {
      NS_ASSERT (false);
    }
  
  //std::cout << "CalculatePe: ber=" << p << ", pe=" << pe << std::endl;
  
  return pe;
}

double
DSRCErrorRateModel::GetFec16QamBer (double snr, uint32_t nbits,
                                    uint32_t bValue) const
{
  NS_LOG_FUNCTION (this << snr << nbits << bValue);
  double ber = Get16QamBer (snr);
  if (ber == 0.0)
    {
      return 1.0;
    }
  double pe = CalculatePe (ber, bValue);
  pe = std::min (pe, 1.0);
  double pms = std::pow (1 - pe, static_cast<double> (nbits));
  return pms;
}

double
DSRCErrorRateModel::GetFec64QamBer (double snr, uint32_t nbits,
                                    uint32_t bValue) const
{
  NS_LOG_FUNCTION (this << snr << nbits << bValue);
  double ber = Get64QamBer (snr);
  if (ber == 0.0)
    {
      return 1.0;
    }
  double pe = CalculatePe (ber, bValue);
  pe = std::min (pe, 1.0);
  double pms = std::pow (1 - pe, static_cast<double> (nbits));
  return pms;
}

double
DSRCErrorRateModel::GetFec256QamBer (double snr, uint32_t nbits,
                                     uint32_t bValue) const
{
  NS_LOG_FUNCTION (this << snr << nbits << bValue);
  double ber = Get256QamBer (snr);
  if (ber == 0.0)
    {
      return 1.0;
    }
  double pe = CalculatePe (ber, bValue);
  pe = std::min (pe, 1.0);
  double pms = std::pow (1 - pe, static_cast<double> (nbits));
  return pms;
}

//double
//DSRCErrorRateModel::GetChunkSuccessRate (WifiMode mode, WifiTxVector txVector, double snr, uint32_t nbits) const
double
DSRCErrorRateModel::GetChunkSuccessRate (WifiMode mode, double snr, uint32_t nbits) const
{
  NS_LOG_FUNCTION (this << mode << snr << nbits); /*<< txVector.GetMode ()*/
  if (mode.GetModulationClass () == WIFI_MOD_CLASS_ERP_OFDM
      || mode.GetModulationClass () == WIFI_MOD_CLASS_OFDM
      || mode.GetModulationClass () == WIFI_MOD_CLASS_HT)
      //|| mode.GetModulationClass () == WIFI_MOD_CLASS_VHT)
    {
      if (mode.GetConstellationSize () == 2)
        {
          if (mode.GetCodeRate () == WIFI_CODE_RATE_1_2)
            {
              return GetFecBpskBer (snr,
                                    nbits,
                                    1); //b value
            }
          else
            {
              return GetFecBpskBer (snr,
                                    nbits,
                                    3); //b value
            }
        }
      else if (mode.GetConstellationSize () == 4)
        {
          if (mode.GetCodeRate () == WIFI_CODE_RATE_1_2)
            {
              return GetFecQpskBer (snr,
                                    nbits,
                                    1); //b value
            }
          else
            {
              return GetFecQpskBer (snr,
                                    nbits,
                                    3); //b value
            }
        }
      else if (mode.GetConstellationSize () == 16)
        {
          if (mode.GetCodeRate () == WIFI_CODE_RATE_1_2)
            {
              return GetFec16QamBer (snr,
                                     nbits,
                                     1); //b value
            }
          else
            {
              return GetFec16QamBer (snr,
                                     nbits,
                                     3); //b value
            }
        }
      else if (mode.GetConstellationSize () == 64)
        {
          if (mode.GetCodeRate () == WIFI_CODE_RATE_2_3)
            {
              return GetFec64QamBer (snr,
                                     nbits,
                                     2); //b value
            }
          else if (mode.GetCodeRate () == WIFI_CODE_RATE_5_6)
            {
              return GetFec64QamBer (snr,
                                     nbits,
                                     5); //b value
            }
          else
            {
              return GetFec64QamBer (snr,
                                     nbits,
                                     3); //b value
            }
        }
      else if (mode.GetConstellationSize () == 256)
        {
          if (mode.GetCodeRate () == WIFI_CODE_RATE_5_6)
            {
              return GetFec256QamBer (snr,
                                      nbits,
                                      5     // b value
                                      );
            }
          else
            {
              return GetFec256QamBer (snr,
                                      nbits,
                                      3     // b value
                                      );
            }
        }
    }
  else if (mode.GetModulationClass () == WIFI_MOD_CLASS_DSSS) //|| mode.GetModulationClass () == WIFI_MOD_CLASS_HR_DSSS)
    {
      //switch (mode.GetDataRate (20, 0, 1))
	  switch (mode.GetDataRate ())
        {
        case 1000000:
          return DsssErrorRateModel::GetDsssDbpskSuccessRate (snr, nbits);
        case 2000000:
          return DsssErrorRateModel::GetDsssDqpskSuccessRate (snr, nbits);
        case 5500000:
          return DsssErrorRateModel::GetDsssDqpskCck5_5SuccessRate (snr, nbits);
        case 11000000:
          return DsssErrorRateModel::GetDsssDqpskCck11SuccessRate (snr, nbits);
        default:
          NS_ASSERT ("undefined DSSS/HR-DSSS datarate");
        }
    }
  return 0;
}

} //namespace ns3
