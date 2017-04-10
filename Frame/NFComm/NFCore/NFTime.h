#ifndef NF_TIME_H
#define NF_TIME_H

#include <vector>
#include <string>

//ĳ�����ĳЩ����
#define CLEAR_HOUR 5

class NFTime
{
public:
    enum TimeType
    {
        //-----------BASE-----------
        YEAR,
        MONTH,//start by 0
        DAY,
        HOUR,
        MINUTE,
        SECOND,
        MILLISECOND,
        //------------EX-------------
        WEEK_SINCE_EPOCH,//start by 0
        DAY_SINCE_EPOCH,//start by 0
        HOUR_SINCE_EPOCH,//start by 0
        DAY_OF_WEEK,//start by 1 ,1 means Sunday
    };

public:
    //���ֺ���
    static const int64_t NSECOND_MS = 1000L;
    static const int64_t NMINUTE_MS = 60 * 1000L;
    static const int64_t NHOUR_MS = 60 * 60 * 1000L;
    static const int64_t NDAY_MS = 24 * 60 * 60 * 1000L;
    static const int64_t NWEEK_MS = 7 * 24 * 60 * 60 * 1000L;
    //������
    static const int64_t NSECOND_S = 1L;
    static const int64_t NMINUTE_S = 60L;
    static const int64_t NHOUR_S = 60 * 60L;
    static const int64_t NDAY_S = 24 * 60 * 60L;
    static const int64_t NWEEK_S = 7 * 24 * 60 * 60L;

    static const int mnDefaultTimeZone = 8;

public:
    NFTime();
    NFTime(int64_t nTime);
    NFTime(int64_t nTime, int nTimeZone);
    NFTime(std::string strTime);
    NFTime(std::string strTime, int nTimeZone);
    NFTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond); //with default(UTC+8) timezone
    NFTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilliSecond, int nTimeZone = mnDefaultTimeZone); //with particular timezone

    //��ʱ��
    std::string GetStr();
    std::string GetStr_YMD();
    std::string GetStr_HMS();
    std::string GetStr_YMDHMS();
    std::string GetStr_YMDHMSM(); //YYYY_MM_DD_hh_mm_ss_ms
    bool FromString(const std::string& strTime);

    static int64_t GetUTCTime();        //��λ����, 0ʱ��ʱ��
    //static int64_t GetNowSecondTime();        //��λ����, �ض�ʱ��ʱ�� (�������ã��ر�ע�⣺��ΪNFTime���õĶ��Ǻ��룬���Ҫ���˽����ֵת��NFTime����Ҫ *1000)
    static int64_t GetNowMillisecond(); //��λ�Ǻ���, �ض�ʱ��ʱ��
    static int64_t GetTimeZoneSecond();
    static int64_t GetTimeZoneMillisecond();

    int64_t GetTime();
    int64_t Get(TimeType type); //��ʱ��(�ر�ע��: ��������mnTime�Ļ����ϼ���ʱ��������)
    int GetTimeZone();

    void SetTime(int64_t nTime);
    static void SetTimeZone(int nTimeZone);

    //��ʱ��
    int64_t GetYear();
    int64_t GetMonth();
    int64_t GetDay();
    int64_t GetHour();
    int64_t GetMinute();
    int64_t GetSecond();

    bool SameMinute(int64_t nTime);
    bool SameHour(int64_t nTime);
    bool SameDay(int64_t nTime);
    bool SameMonth(int64_t nTime);
    bool SameYear(int64_t nTime);
    bool SameWeek(int64_t nTime);

    int GetDayOfWeek();
    int GetWeekOfMonth();
    int GetDayOfYear();
    int GetWeekOfYear();

private:
    void InitWithYMDHMSM(std::string strTime);
    void InitWithYMDHMSM(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilliSecond);
    //int64_t GetFixTime();//add timeZone value
    bool IsLeapYear(int nYear);
    int Split(const std::string& str, std::vector<std::string>& ret_, std::string sep);

private:
    int64_t mnTime;
    static int mnTimeZone; //Ĭ�ϸ�UTC+8
};


#endif // NF_TIME_H
