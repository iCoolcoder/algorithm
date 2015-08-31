/*
 * Author: gaosiyu@youku.com
 */
#include "rtcp.h"
#include <iostream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <mmsystem.h>
#include <timeapi.h>
#endif
using namespace std;

#define LOG(sev) cout

namespace avformat
{
    uint32_t MidNtp(uint32_t ntp_sec, uint32_t ntp_frac)
    {
        return (ntp_sec << 16) + (ntp_frac >> 16);
    }

    int64_t Clock::NtpToMs(uint32_t ntp_secs, uint32_t ntp_frac)
    {
        const double ntp_frac_ms = static_cast<double>(ntp_frac) / kNtpFracPerMs;
        return 1000 * static_cast<int64_t>(ntp_secs)+
            static_cast<int64_t>(ntp_frac_ms + 0.5);
    }


    int64_t QueryOsForTicks()
    {
        int64_t result;
#ifdef WIN32
        // TODO(wu): Remove QueryPerformanceCounter implementation.
#ifdef USE_QUERY_PERFORMANCE_COUNTER
        // QueryPerformanceCounter returns the value from the TSC which is
        // incremented at the CPU frequency. The algorithm used requires
        // the CPU frequency to be constant. Technology like speed stepping
        // which has variable CPU frequency will therefore yield unpredictable,
        // incorrect time estimations.
        LARGE_INTEGER qpcnt;
        QueryPerformanceCounter(&qpcnt);
        result.ticks_ = qpcnt.QuadPart;
#else
        static volatile LONG last_time_get_time = 0;
        static volatile int64_t num_wrap_time_get_time = 0;
        volatile LONG* last_time_get_time_ptr = &last_time_get_time;
        DWORD now = timeGetTime();
        // Atomically update the last gotten time
        DWORD old = InterlockedExchange(last_time_get_time_ptr, now);
        if (now < old) {
            // If now is earlier than old, there may have been a race between
            // threads.
            // 0x0fffffff ~3.1 days, the code will not take that long to execute
            // so it must have been a wrap around.
            if (old > 0xf0000000 && now < 0x0fffffff) {
                num_wrap_time_get_time++;
            }
        }
        result = now + (num_wrap_time_get_time << 32);
#endif
#elif defined(WEBRTC_LINUX)
        struct timespec ts;
        // TODO(wu): Remove CLOCK_REALTIME implementation.
#ifdef WEBRTC_CLOCK_TYPE_REALTIME
        clock_gettime(CLOCK_REALTIME, &ts);
#else
        clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
        result = 1000000000LL * static_cast<int64_t>(ts.tv_sec) +
            static_cast<int64_t>(ts.tv_nsec);
#elif defined(WEBRTC_MAC)
        static mach_timebase_info_data_t timebase;
        if (timebase.denom == 0) {
            // Get the timebase if this is the first time we run.
            // Recommended by Apple's QA1398.
            kern_return_t retval = mach_timebase_info(&timebase);
            if (retval != KERN_SUCCESS) {
                // TODO(wu): Implement CHECK similar to chrome for all the platforms.
                // Then replace this with a CHECK(retval == KERN_SUCCESS);
#ifndef WEBRTC_IOS
                asm("int3");
#else
                __builtin_trap();
#endif  // WEBRTC_IOS
            }
        }
        // Use timebase to convert absolute time tick units into nanoseconds.
        result.ticks_ = mach_absolute_time() * timebase.numer / timebase.denom;
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        result = 1000000LL * static_cast<int64_t>(tv.tv_sec) +
            static_cast<int64_t>(tv.tv_usec);
#endif
        return result;
    }


    class RealTimeClock : public Clock
    {
        // Return a timestamp in milliseconds relative to some arbitrary source; the
        // source is fixed for this clock.
        int64_t TimeInMilliseconds() const
        {
            //        return TickTime::MillisecondTimestamp();
            int64_t  ticks = QueryOsForTicks();
#ifdef WIN32
#ifdef USE_QUERY_PERFORMANCE_COUNTER
            LARGE_INTEGER qpfreq;
            QueryPerformanceFrequency(&qpfreq);
            return (ticks * 1000) / qpfreq.QuadPart;
#else
            return ticks;
#endif
#elif defined(WEBRTC_LINUX) || defined(WEBRTC_MAC)
            return ticks / 1000000LL;
#else
            return ticks / 1000LL;
#endif
        }

        // Return a timestamp in microseconds relative to some arbitrary source; the
        // source is fixed for this clock.
        int64_t TimeInMicroseconds() const
        {
            //        return TickTime::MicrosecondTimestamp();
            int64_t  ticks = QueryOsForTicks();
#ifdef WIN32
#ifdef USE_QUERY_PERFORMANCE_COUNTER
            LARGE_INTEGER qpfreq;
            QueryPerformanceFrequency(&qpfreq);
            return (ticks * 1000) / (qpfreq.QuadPart / 1000);
#else
            return ticks * 1000LL;
#endif
#elif defined(WEBRTC_LINUX) || defined(WEBRTC_MAC)
            return ticks / 1000LL;
#else
            return ticks;
#endif
        }

        // Retrieve an NTP absolute timestamp in seconds and fractions of a second.
        void CurrentNtp(uint32_t& seconds, uint32_t& fractions) const
        {
            timeval tv = CurrentTimeVal();
            double microseconds_in_seconds;
            Adjust(tv, &seconds, &microseconds_in_seconds);
            fractions = static_cast<uint32_t>(
                microseconds_in_seconds * kMagicNtpFractionalUnit + 0.5);
        }

        // Retrieve an NTP absolute timestamp in milliseconds.
        int64_t CurrentNtpInMilliseconds() const
        {
            timeval tv = CurrentTimeVal();
            uint32_t seconds;
            double microseconds_in_seconds;
            Adjust(tv, &seconds, &microseconds_in_seconds);
            return 1000 * static_cast<int64_t>(seconds)+
                static_cast<int64_t>(1000.0 * microseconds_in_seconds + 0.5);
        }

    protected:
        virtual timeval CurrentTimeVal() const = 0;

        static void Adjust(const timeval& tv, uint32_t* adjusted_s,
            double* adjusted_us_in_s) {
            *adjusted_s = tv.tv_sec + kNtpJan1970;
            *adjusted_us_in_s = tv.tv_usec / 1e6;

            if (*adjusted_us_in_s >= 1) {
                *adjusted_us_in_s -= 1;
                ++*adjusted_s;
            }
            else if (*adjusted_us_in_s < -1) {
                *adjusted_us_in_s += 1;
                --*adjusted_s;
            }
        }
    };


#ifdef WIN32
    // TODO(pbos): Consider modifying the implementation to synchronize itself
    // against system time (update ref_point_, make it non-const) periodically to
    // prevent clock drift.
    class WindowsRealTimeClock : public RealTimeClock {
    public:
        WindowsRealTimeClock()
            : last_time_ms_(0),
            num_timer_wraps_(0),
            ref_point_(GetSystemReferencePoint()) {}

        virtual ~WindowsRealTimeClock() {}

    protected:
        struct ReferencePoint {
            FILETIME file_time;
            LARGE_INTEGER counter_ms;
        };

        timeval CurrentTimeVal() const override {
            const uint64_t FILETIME_1970 = 0x019db1ded53e8000;

            FILETIME StartTime;
            uint64_t Time;
            struct timeval tv;

            // We can't use query performance counter since they can change depending on
            // speed stepping.
            GetTime(&StartTime);

            Time = (((uint64_t)StartTime.dwHighDateTime) << 32) +
                (uint64_t)StartTime.dwLowDateTime;

            // Convert the hecto-nano second time to tv format.
            Time -= FILETIME_1970;

            tv.tv_sec = (uint32_t)(Time / (uint64_t)10000000);
            tv.tv_usec = (uint32_t)((Time % (uint64_t)10000000) / 10);
            return tv;
        }

        void GetTime(FILETIME* current_time) const {
            DWORD t;
            LARGE_INTEGER elapsed_ms;
            {
                //rtc::CritScope lock(&crit_);
                // time MUST be fetched inside the critical section to avoid non-monotonic
                // last_time_ms_ values that'll register as incorrect wraparounds due to
                // concurrent calls to GetTime.
                t = timeGetTime();
                if (t < last_time_ms_)
                    num_timer_wraps_++;
                last_time_ms_ = t;
                elapsed_ms.HighPart = num_timer_wraps_;
            }
            elapsed_ms.LowPart = t;
            elapsed_ms.QuadPart = elapsed_ms.QuadPart - ref_point_.counter_ms.QuadPart;

            // Translate to 100-nanoseconds intervals (FILETIME resolution)
            // and add to reference FILETIME to get current FILETIME.
            ULARGE_INTEGER filetime_ref_as_ul;
            filetime_ref_as_ul.HighPart = ref_point_.file_time.dwHighDateTime;
            filetime_ref_as_ul.LowPart = ref_point_.file_time.dwLowDateTime;
            filetime_ref_as_ul.QuadPart +=
                static_cast<ULONGLONG>((elapsed_ms.QuadPart) * 1000 * 10);

            // Copy to result
            current_time->dwHighDateTime = filetime_ref_as_ul.HighPart;
            current_time->dwLowDateTime = filetime_ref_as_ul.LowPart;
        }

        static ReferencePoint GetSystemReferencePoint() {
            ReferencePoint ref = { 0 };
            FILETIME ft0 = { 0 };
            FILETIME ft1 = { 0 };
            // Spin waiting for a change in system time. As soon as this change happens,
            // get the matching call for timeGetTime() as soon as possible. This is
            // assumed to be the most accurate offset that we can get between
            // timeGetTime() and system time.

            // Set timer accuracy to 1 ms.
            timeBeginPeriod(1);
            GetSystemTimeAsFileTime(&ft0);
            do {
                GetSystemTimeAsFileTime(&ft1);

                ref.counter_ms.QuadPart = timeGetTime();
                Sleep(0);
            } while ((ft0.dwHighDateTime == ft1.dwHighDateTime) &&
                (ft0.dwLowDateTime == ft1.dwLowDateTime));
            ref.file_time = ft1;
            timeEndPeriod(1);
            return ref;
        }

        // mutable as time-accessing functions are const.
        //mutable rtc::CriticalSection crit_;
        mutable DWORD last_time_ms_;
        mutable LONG num_timer_wraps_;
        const ReferencePoint ref_point_;
    };

#elif ((defined WEBRTC_LINUX) || (defined WEBRTC_MAC))
    class UnixRealTimeClock : public RealTimeClock {
    public:
        UnixRealTimeClock() {}

        ~UnixRealTimeClock()  {}

    protected:
        timeval CurrentTimeVal() const  {
            struct timeval tv;
            struct timezone tz;
            tz.tz_minuteswest = 0;
            tz.tz_dsttime = 0;
            gettimeofday(&tv, &tz);
            return tv;
        }
    };
#endif

#ifdef WIN32
    static WindowsRealTimeClock* volatile g_shared_clock = nullptr;
#endif
    avformat::Clock* avformat::Clock::GetRealTimeClock() {
#ifdef WIN32
        // This read relies on volatile read being atomic-load-acquire. This is
        // true in MSVC since at least 2005:
        // "A read of a volatile object (volatile read) has Acquire semantics"
        if (g_shared_clock != nullptr)
            return g_shared_clock;
        WindowsRealTimeClock* clock = new WindowsRealTimeClock;
        if (InterlockedCompareExchangePointer(
            reinterpret_cast<void* volatile*>(&g_shared_clock), clock, nullptr) !=
            nullptr) {
            // g_shared_clock was assigned while we constructed/tried to assign our
            // instance, delete our instance and use the existing one.
            delete clock;
        }
        return g_shared_clock;
#elif defined(WEBRTC_LINUX) || defined(WEBRTC_MAC)
        static UnixRealTimeClock clock;
        return &clock;
#else
        return NULL;
#endif
    }

    RawPacket RTCP::generate_rtcp_packet(RtcpPacket *rtcp_packet)
    {
        return rtcp_packet->Build();
    }

    void RTCP::parse_rtcp_packet(const uint8_t *data, uint32_t len, RTCPPacketInformation& rtcpPacketInformation)
    {
        RTCPParserV2 *rtcpParser = new RTCPParserV2(data, len, true);

        //_lastReceived = _clock->TimeInMilliseconds();

        //if (packet_type_counter_.first_packet_time_ms == -1) {
        //    packet_type_counter_.first_packet_time_ms = _lastReceived;
        //}

        RTCPPacketTypes pktType = rtcpParser->Begin();
        while (pktType != kInvalid) {
            // Each "case" is responsible for iterate the parser to the
            // next top level packet.
            switch (pktType)
            {
            case kSr:
            case kRr:
                HandleSenderReceiverReport(*rtcpParser, rtcpPacketInformation);
                break;
            case kSdes:
                HandleSDES(*rtcpParser);
                break;
            case kBye:
                HandleBYE(*rtcpParser);
                break;
            case kRtpfbNack:
                HandleNACK(*rtcpParser, rtcpPacketInformation);
                break;
            case kApp:
                // generic application messages
                HandleAPP(*rtcpParser, rtcpPacketInformation);
                break;
            case kAppItem:
                // generic application messages
                HandleAPPItem(*rtcpParser, rtcpPacketInformation);
                break;
            default:
                rtcpParser->Iterate();
                break;
            }
            pktType = rtcpParser->PacketType();
        }

        //if (packet_type_counter_observer_ != NULL) {
        //    packet_type_counter_observer_->RtcpPacketTypesCounterUpdated(
        //        main_ssrc_, packet_type_counter_);
        //}
    }

    void RTCP::HandleSenderReceiverReport(RTCPParserV2& rtcpParser,
        RTCPPacketInformation& rtcpPacketInformation)
    {
        RTCPPacketTypes rtcpPacketType = rtcpParser.PacketType();
        const RTCPPacket& rtcpPacket = rtcpParser.Packet();

        assert((rtcpPacketType == kRr) ||
            (rtcpPacketType == kSr));

        // SR.SenderSSRC
        // The synchronization source identifier for the originator of this SR packet

        // rtcpPacket.RR.SenderSSRC
        // The source of the packet sender, same as of SR? or is this a CE?

        const uint32_t remoteSSRC = (rtcpPacketType == kRr)
            ? rtcpPacket.RR.SenderSSRC
            : rtcpPacket.SR.SenderSSRC;

        rtcpPacketInformation.remoteSSRC = remoteSSRC;

        RTCPReceiveInformation* ptrReceiveInfo = CreateReceiveInformation(remoteSSRC);
        if (!ptrReceiveInfo)
        {
            rtcpParser.Iterate();
            return;
        }

        
        _remoteSSRC = remoteSSRC;


        if (rtcpPacketType == kSr) {
            //TRACE_EVENT_INSTANT2(TRACE_DISABLED_BY_DEFAULT("webrtc_rtp"), "SR",
            //    "remote_ssrc", remoteSSRC, "ssrc", main_ssrc_);

            if (_remoteSSRC == remoteSSRC) // have I received RTP packets from this party
            {
                // only signal that we have received a SR when we accept one
                rtcpPacketInformation.rtcpPacketTypeFlags |= kRtcpSr;

                rtcpPacketInformation.ntp_secs = rtcpPacket.SR.NTPMostSignificant;
                rtcpPacketInformation.ntp_frac = rtcpPacket.SR.NTPLeastSignificant;
                rtcpPacketInformation.rtp_timestamp = rtcpPacket.SR.RTPTimestamp;
                rtcpPacketInformation.sendPacketCount = rtcpPacket.SR.SenderPacketCount;
                rtcpPacketInformation.sendOctetCount = rtcpPacket.SR.SenderOctetCount;

                // We will only store the send report from one source, but
                // we will store all the receive block

                // Save the NTP time of this report
                _remoteSenderInfo.NTPseconds = rtcpPacket.SR.NTPMostSignificant;
                _remoteSenderInfo.NTPfraction = rtcpPacket.SR.NTPLeastSignificant;
                _remoteSenderInfo.RTPtimeStamp = rtcpPacket.SR.RTPTimestamp;
                _remoteSenderInfo.sendPacketCount = rtcpPacket.SR.SenderPacketCount;
                _remoteSenderInfo.sendOctetCount = rtcpPacket.SR.SenderOctetCount;

                _clock->CurrentNtp(_lastReceivedSRNTPsecs, _lastReceivedSRNTPfrac);
            }
            else
            {
                rtcpPacketInformation.rtcpPacketTypeFlags |= kRtcpRr;
            }
        }
        else
        {
            //TRACE_EVENT_INSTANT2(TRACE_DISABLED_BY_DEFAULT("webrtc_rtp"), "RR",
            //    "remote_ssrc", remoteSSRC, "ssrc", main_ssrc_);

            rtcpPacketInformation.rtcpPacketTypeFlags |= kRtcpRr;
        }
        UpdateReceiveInformation(*ptrReceiveInfo);

        rtcpPacketType = rtcpParser.Iterate();

        while (rtcpPacketType == kReportBlockItem) {
            HandleReportBlock(rtcpPacket, rtcpPacketInformation, remoteSSRC);
            rtcpPacketType = rtcpParser.Iterate();
        }
    }

    RTCPReceiveInformation* RTCP::CreateReceiveInformation(uint32_t remoteSSRC) {

        std::map<uint32_t, RTCPReceiveInformation*>::iterator it =
            _receivedInfoMap.find(remoteSSRC);

        if (it != _receivedInfoMap.end()) {
            return it->second;
        }
        RTCPReceiveInformation* receiveInfo = new RTCPReceiveInformation;
        _receivedInfoMap[remoteSSRC] = receiveInfo;
        return receiveInfo;
    }

    // no need for critsect we have _criticalSectionRTCPReceiver
    void RTCP::HandleReportBlock(
        const RTCPPacket& rtcpPacket,
        RTCPPacketInformation& rtcpPacketInformation,
        uint32_t remoteSSRC)
        //EXCLUSIVE_LOCKS_REQUIRED(_criticalSectionRTCPReceiver) 
    {
        // This will be called once per report block in the RTCP packet.
        // We filter out all report blocks that are not for us.
        // Each packet has max 31 RR blocks.
        //
        // We can calc RTT if we send a send report and get a report block back.

        // |rtcpPacket.ReportBlockItem.SSRC| is the SSRC identifier of the source to
        // which the information in this reception report block pertains.

        // Filter out all report blocks that are not for us.
        if (registered_ssrcs_.find(rtcpPacket.ReportBlockItem.SSRC) ==
            registered_ssrcs_.end()) {
            // This block is not for us ignore it.
            return;
        }

        // To avoid problem with acquiring _criticalSectionRTCPSender while holding
        // _criticalSectionRTCPReceiver.
        //FIXME: no rtpRtcp
        int64_t sendTimeMS = 0;//_rtpRtcp.SendTimeOfSendReport(rtcpPacket.ReportBlockItem.LastSR);


        RTCPReportBlockInformation* reportBlock =
            CreateOrGetReportBlockInformation(remoteSSRC,
            rtcpPacket.ReportBlockItem.SSRC);
        if (reportBlock == NULL) {
            LOG(LS_WARNING) << "Failed to CreateReportBlockInformation("
                << remoteSSRC << ")";
            return;
        }

        _lastReceivedRrMs = _clock->TimeInMilliseconds();
        const RTCPPacketReportBlockItem& rb = rtcpPacket.ReportBlockItem;
        reportBlock->remoteReceiveBlock.remoteSSRC = remoteSSRC;
        reportBlock->remoteReceiveBlock.sourceSSRC = rb.SSRC;
        reportBlock->remoteReceiveBlock.fractionLost = rb.FractionLost;
        reportBlock->remoteReceiveBlock.cumulativeLost =
            rb.CumulativeNumOfPacketsLost;
        if (rb.ExtendedHighestSequenceNumber >
            reportBlock->remoteReceiveBlock.extendedHighSeqNum) {
            // We have successfully delivered new RTP packets to the remote side after
            // the last RR was sent from the remote side.
            _lastIncreasedSequenceNumberMs = _lastReceivedRrMs;
        }
        reportBlock->remoteReceiveBlock.extendedHighSeqNum =
            rb.ExtendedHighestSequenceNumber;
        reportBlock->remoteReceiveBlock.jitter = rb.Jitter;
        reportBlock->remoteReceiveBlock.delaySinceLastSR = rb.DelayLastSR;
        reportBlock->remoteReceiveBlock.lastSR = rb.LastSR;

        if (rtcpPacket.ReportBlockItem.Jitter > reportBlock->remoteMaxJitter) {
            reportBlock->remoteMaxJitter = rtcpPacket.ReportBlockItem.Jitter;
        }

        uint32_t delaySinceLastSendReport =
            rtcpPacket.ReportBlockItem.DelayLastSR;

        // local NTP time when we received this
        uint32_t lastReceivedRRNTPsecs = 0;
        uint32_t lastReceivedRRNTPfrac = 0;

        _clock->CurrentNtp(lastReceivedRRNTPsecs, lastReceivedRRNTPfrac);

        // time when we received this in MS
        int64_t receiveTimeMS = Clock::NtpToMs(lastReceivedRRNTPsecs,
            lastReceivedRRNTPfrac);

        // Estimate RTT
        uint32_t d = (delaySinceLastSendReport & 0x0000ffff) * 1000;
        d /= 65536;
        d += ((delaySinceLastSendReport & 0xffff0000) >> 16) * 1000;

        int64_t RTT = 0;

        if (sendTimeMS > 0) {
            RTT = receiveTimeMS - d - sendTimeMS;
            if (RTT <= 0) {
                RTT = 1;
            }
            if (RTT > reportBlock->maxRTT) {
                // store max RTT
                reportBlock->maxRTT = RTT;
            }
            if (reportBlock->minRTT == 0) {
                // first RTT
                reportBlock->minRTT = RTT;
            }
            else if (RTT < reportBlock->minRTT) {
                // Store min RTT
                reportBlock->minRTT = RTT;
            }
            // store last RTT
            reportBlock->RTT = RTT;

            // store average RTT
            if (reportBlock->numAverageCalcs != 0) {
                float ac = static_cast<float>(reportBlock->numAverageCalcs);
                float newAverage =
                    ((ac / (ac + 1)) * reportBlock->avgRTT) + ((1 / (ac + 1)) * RTT);
                reportBlock->avgRTT = static_cast<int64_t>(newAverage + 0.5f);
            }
            else {
                // first RTT
                reportBlock->avgRTT = RTT;
            }
            reportBlock->numAverageCalcs++;
        }

        //TRACE_COUNTER_ID1(TRACE_DISABLED_BY_DEFAULT("webrtc_rtp"), "RR_RTT", rb.SSRC,
        //    RTT);

        rtcpPacketInformation.AddReportInfo(*reportBlock);
    }

    RTCPReportBlockInformation* RTCP::CreateOrGetReportBlockInformation(
        uint32_t remote_ssrc,
        uint32_t source_ssrc) {
        RTCPReportBlockInformation* info =
            GetReportBlockInformation(remote_ssrc, source_ssrc);
        if (info == NULL) {
            info = new RTCPReportBlockInformation;
            _receivedReportBlockMap[source_ssrc][remote_ssrc] = info;
        }
        return info;
    }
    /*
    RTCPReportBlockInformation* RTCP::GetReportBlockInformation(
    uint32_t remote_ssrc,
    uint32_t source_ssrc) const {
    ReportBlockMap::const_iterator it = _receivedReportBlockMap.find(source_ssrc);
    if (it == _receivedReportBlockMap.end()) {
    return NULL;
    }
    const ReportBlockInfoMap* info_map = &(it->second);
    ReportBlockInfoMap::const_iterator it_info = info_map->find(remote_ssrc);
    if (it_info == info_map->end()) {
    return NULL;
    }
    return it_info->second;
    }
    */
    RTCPCnameInformation*
        RTCP::CreateCnameInformation(uint32_t remoteSSRC)
    {

            std::map<uint32_t, RTCPCnameInformation*>::iterator it =
                _receivedCnameMap.find(remoteSSRC);

            if (it != _receivedCnameMap.end())
            {
                return it->second;
            }
            RTCPCnameInformation* cnameInfo = new RTCPCnameInformation;
            memset(cnameInfo->name, 0, RTCP_CNAME_SIZE);
            _receivedCnameMap[remoteSSRC] = cnameInfo;
            return cnameInfo;
        }

    // no need for critsect we have _criticalSectionRTCPReceiver
    void RTCP::HandleSDES(RTCPParserV2& rtcpParser)
    {
        RTCPPacketTypes pktType = rtcpParser.Iterate();
        while (pktType == kSdesChunk)
        {
            HandleSDESChunk(rtcpParser);
            pktType = rtcpParser.Iterate();
        }
    }

    // no need for critsect we have _criticalSectionRTCPReceiver
    void RTCP::HandleSDESChunk(RTCPParserV2& rtcpParser)
    {
        const RTCPPacket& rtcpPacket = rtcpParser.Packet();
        RTCPCnameInformation* cnameInfo =
            CreateCnameInformation(rtcpPacket.CName.SenderSSRC);
        assert(cnameInfo);

        cnameInfo->name[RTCP_CNAME_SIZE - 1] = 0;
        strncpy(cnameInfo->name, rtcpPacket.CName.CName, RTCP_CNAME_SIZE - 1);
        {
            if (stats_callback_ != NULL)
            {
                stats_callback_->CNameChanged(rtcpPacket.CName.CName,
                    rtcpPacket.CName.SenderSSRC);
            }
        }
    }

    // no need for critsect we have _criticalSectionRTCPReceiver
    void RTCP::HandleNACK(RTCPParserV2& rtcpParser,
        RTCPPacketInformation& rtcpPacketInformation)
    {
        const RTCPPacket& rtcpPacket = rtcpParser.Packet();

        rtcpPacketInformation.remoteSSRC =  rtcpPacket.NACK.SenderSSRC;
        if (receiver_only_ || main_ssrc_ != rtcpPacket.NACK.MediaSSRC) {
            // Not to us.
            rtcpParser.Iterate();
            return;
        }
        rtcpPacketInformation.ResetNACKPacketIdArray();

        RTCPPacketTypes pktType = rtcpParser.Iterate();
        while (pktType == kRtpfbNackItem)
        {
            HandleNACKItem(rtcpPacket, rtcpPacketInformation);
            pktType = rtcpParser.Iterate();
        }

        if (rtcpPacketInformation.rtcpPacketTypeFlags & kRtcpNack)
        {
            ++packet_type_counter_.nack_packets;
            packet_type_counter_.nack_requests = nack_stats_.requests();
            packet_type_counter_.unique_nack_requests = nack_stats_.unique_requests();
        }
    }

    // no need for critsect we have _criticalSectionRTCPReceiver
    void RTCP::HandleNACKItem(const RTCPPacket& rtcpPacket,
        RTCPPacketInformation& rtcpPacketInformation) {
        rtcpPacketInformation.AddNACKPacket(rtcpPacket.NACKItem.PacketID);
        nack_stats_.ReportRequest(rtcpPacket.NACKItem.PacketID);

        uint16_t bitMask = rtcpPacket.NACKItem.BitMask;
        if (bitMask) {
            for (int i = 1; i <= 16; ++i) {
                if (bitMask & 0x01) {
                    rtcpPacketInformation.AddNACKPacket(rtcpPacket.NACKItem.PacketID + i);
                    nack_stats_.ReportRequest(rtcpPacket.NACKItem.PacketID + i);
                }
                bitMask = bitMask >> 1;
            }
        }
        rtcpPacketInformation.rtcpPacketTypeFlags |= kRtcpNack;
    }

    // no need for critsect we have _criticalSectionRTCPReceiver
    void RTCP::HandleBYE(RTCPParserV2& rtcpParser)
    {
        const RTCPPacket& rtcpPacket = rtcpParser.Packet();

        // clear our lists
        ReportBlockMap::iterator it = _receivedReportBlockMap.begin();
        for (; it != _receivedReportBlockMap.end(); ++it) {
            ReportBlockInfoMap* info_map = &(it->second);
            ReportBlockInfoMap::iterator it_info = info_map->find(
                rtcpPacket.BYE.SenderSSRC);
            if (it_info != info_map->end()) {
                delete it_info->second;
                info_map->erase(it_info);
            }
        }

        //  we can't delete it due to TMMBR
        std::map<uint32_t, RTCPReceiveInformation*>::iterator receiveInfoIt =
            _receivedInfoMap.find(rtcpPacket.BYE.SenderSSRC);

        if (receiveInfoIt != _receivedInfoMap.end()) {
            receiveInfoIt->second->readyForDelete = true;
        }

        std::map<uint32_t, RTCPCnameInformation*>::iterator cnameInfoIt =
            _receivedCnameMap.find(rtcpPacket.BYE.SenderSSRC);

        if (cnameInfoIt != _receivedCnameMap.end()) {
            delete cnameInfoIt->second;
            _receivedCnameMap.erase(cnameInfoIt);
        }
        rtcpParser.Iterate();
    }

    RTCPReceiveInformation* RTCP::GetReceiveInformation(uint32_t remoteSSRC)
    {

        std::map<uint32_t, RTCPReceiveInformation*>::iterator it =
            _receivedInfoMap.find(remoteSSRC);
        if (it == _receivedInfoMap.end()) {
            return NULL;
        }
        return it->second;
    }

    void RTCP::HandleAPP(RTCPParserV2& rtcpParser,
        RTCPPacketInformation& rtcpPacketInformation) {
        const RTCPPacket& rtcpPacket = rtcpParser.Packet();

        rtcpPacketInformation.rtcpPacketTypeFlags |= kRtcpApp;
        rtcpPacketInformation.applicationSubType = rtcpPacket.APP.SubType;
        rtcpPacketInformation.applicationName = rtcpPacket.APP.Name;

        rtcpParser.Iterate();
    }

    void RTCP::HandleAPPItem(RTCPParserV2& rtcpParser,
        RTCPPacketInformation& rtcpPacketInformation) {
        const RTCPPacket& rtcpPacket = rtcpParser.Packet();

        rtcpPacketInformation.AddApplicationData(rtcpPacket.APP.Data, rtcpPacket.APP.Size);

        rtcpParser.Iterate();
    }

    RTCPReportBlockInformation* RTCP::GetReportBlockInformation(
        uint32_t remote_ssrc,
        uint32_t source_ssrc) const {
        ReportBlockMap::const_iterator it = _receivedReportBlockMap.find(source_ssrc);
        if (it == _receivedReportBlockMap.end()) {
            return NULL;
        }
        const ReportBlockInfoMap* info_map = &(it->second);
        ReportBlockInfoMap::const_iterator it_info = info_map->find(remote_ssrc);
        if (it_info == info_map->end()) {
            return NULL;
        }
        return it_info->second;
    }
    void RTCP::UpdateReceiveInformation(
        RTCPReceiveInformation& receiveInformation) {
        // Update that this remote is alive
        receiveInformation.lastTimeReceived = _clock->TimeInMilliseconds();
    }


    RTCP::RTCP(avformat::Clock* clock) : receiver_only_(false), _clock(clock),
        //packet_type_counter_observer_(NULL),
        //_method(kRtcpOff),
        //_lastReceived(0),
        main_ssrc_(0),
        _remoteSSRC(0),
        _remoteSenderInfo(),
        _lastReceivedSRNTPsecs(0),
        _lastReceivedSRNTPfrac(0),
        _lastReceivedXRNTPsecs(0),
        _lastReceivedXRNTPfrac(0),
        _receivedInfoMap(),
        //_packetTimeOutMS(0),
        _lastReceivedRrMs(0),
        _lastIncreasedSequenceNumberMs(0),
        stats_callback_(NULL){

    }

    RTCPCnameInformation* RTCP::GetCnameInformation(uint32_t remoteSSRC) const
    {
        std::map<uint32_t, RTCPCnameInformation*>::const_iterator it =
            _receivedCnameMap.find(remoteSSRC);

        if (it == _receivedCnameMap.end()) {
            return NULL;
        }
        return it->second;
    }

    RTCP::~RTCP()
    {

    }

}




