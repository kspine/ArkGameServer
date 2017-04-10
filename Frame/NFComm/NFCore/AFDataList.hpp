// -------------------------------------------------------------------------
//    @FileName         :    AFDataList.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFDataList
//
// -------------------------------------------------------------------------

#ifndef NFI_DATALIST_H
#define NFI_DATALIST_H

#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>
#include <map>
#include <typeinfo>
#include <iomanip>
#include <functional>
#include <iostream>
#include <fstream>
#include <memory>

#ifdef HAVE_BOOST
#include <boost/variant.hpp>
#else
#include "common/variant.hpp"
#endif

#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "Math/Vector3.hpp"

/**
 * @enum    TDATA_TYPE
 *
 * @brief   Values that represent tdata types.
 */

enum TDATA_TYPE
{
    TDATA_UNKNOWN,  // 未知
    TDATA_INT,      // int64_t
    TDATA_DOUBLE,   // double
    TDATA_STRING,   // string
    TDATA_OBJECT,   // NFGUID
    TDATA_POINT,    // Point3D
    TDATA_MAX,
};


/** @brief   The null int. */
const static NFINT64 NULL_INT = 0;
/** @brief   The null double. */
const static double NULL_DOUBLE = 0.0;
/** @brief   The null string. */
const static std::string NULL_STR = "";
/** @brief   Unique identifier for the null. */
const static NFGUID NULL_GUID = NULL_GUID;
/** @brief   The null point. */
const static Point3D NULL_POINT = Point3D();

/**
 * @class   AFDataList
 *
 * @brief   Vector of TData.
 *
 * @author  Nick Yang
 * @date    2016/11/16
 */

class AFDataList
{
public:

    /**
     * @struct  TData
     *
     * @brief   A special variant data that can represent different data type(now have 5 types, int64_t, double, string, NFGUID, Point3D).
     *
     * @author  Nick Yang
     * @date    2016/11/12
     */

    struct TData
    {
    public:

#ifdef HAVE_BOOST
        using variant_type = boost::variant<NFINT64, double, std::string, NFGUID, Point3D>;
#else
        using variant_type = mapbox::util::variant<NFINT64, double, std::string, NFGUID, Point3D>;
#endif // USE_BOOST

    public:

        /**
         * @fn  TData()
         *
         * @brief   Default constructor.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         */

        TData()
        {
            nType = TDATA_UNKNOWN;
        }

        /**
         * @fn  TData(TDATA_TYPE eType)
         *
         * @brief   Constructor.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   eType   The type.
         */

        TData(TDATA_TYPE eType)
        {
            nType = eType;
        }

        /**
         * @fn  TData(const TData& value)
         *
         * @brief   Constructor.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   value   The value.
         */

        TData(const TData& value)
        {
            nType = value.nType;
            variantData = value.variantData;
        }

        /**
         * @fn  ~TData()
         *
         * @brief   Destructor.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         */

        ~TData()
        {
            nType = TDATA_UNKNOWN;
        }

        /**
         * @fn  inline bool operator==(const TData& src) const
         *
         * @brief   Equality operator.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   src Source for the.
         *
         * @return  True if the parameters are considered equivalent.
         */

        inline bool operator==(const TData& src) const
        {
            //&& src.variantData == variantData
            if(src.GetType() == GetType())
            {
                switch(GetType())
                {
                case TDATA_INT:
                    {
                        if(src.GetInt() == GetInt())
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_DOUBLE:
                    {
                        double dValue = GetDouble() - src.GetDouble();
                        if(IsZeroDouble(dValue))
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_STRING:
                    {
                        if(src.GetString() == GetString())
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_OBJECT:
                    {
                        if(src.GetObject() == GetObject())
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_POINT:
                    {
                        if(src.GetPoint() == GetPoint())
                        {
                            return true;
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            return false;
        }

        /**
         * @fn  void Reset()
         *
         * @brief   Resets this object.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         */

        void Reset()
        {
            nType = TDATA_UNKNOWN;
        }

        /**
         * @fn  bool IsNullValue() const
         *
         * @brief   Query if this object is null value.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  True if null value, false if not.
         */

        bool IsNullValue() const
        {
            bool bChanged = false;

            switch(GetType())
            {
            case TDATA_INT:
                {
                    if(0 != GetInt())
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_DOUBLE:
                {
                    double dValue = GetDouble();
                    if(!IsZeroDouble(dValue))
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_STRING:
                {
                    const std::string& strData = GetString();
                    if(!strData.empty())
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_OBJECT:
                {
                    if(!GetObject().IsNull())
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_POINT:
                {
                    if(!GetPoint().IsZero())
                    {
                        bChanged = true;
                    }
                }
                break;
            default:
                break;
            }

            return !bChanged;
        }

        /**
         * @fn  TDATA_TYPE GetType() const
         *
         * @brief   Gets the type.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  The type.
         */

        TDATA_TYPE GetType() const
        {
            return nType;
        }

        /**
         * @fn  void SetType(TDATA_TYPE type)
         *
         * @brief   Sets a type.
         *
         * @author  Nick Yang
         * @date    2016/11/19
         *
         * @param   type    The type.
         */

        void SetType(TDATA_TYPE type)
        {
            nType = type;
        }

        /**
         * @fn  void SetInt(const NFINT64 var)
         *
         * @brief   Sets a double.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   var The variable.
         */

        void SetInt(const NFINT64 var)
        {
            if(nType == TDATA_INT || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_INT;
                variantData = (NFINT64)var;
            }
        }

        /**
         * @fn  void SetDouble(const double var)
         *
         * @brief   Sets a double.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   var The variable.
         */

        void SetDouble(const double var)
        {
            if(nType == TDATA_DOUBLE || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_DOUBLE;
                variantData = (double)var;
            }
        }

        /**
         * @fn  void SetString(const std::string& var)
         *
         * @brief   Sets a string.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   var The variable.
         */

        void SetString(const std::string& var)
        {
            if(nType == TDATA_STRING || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_STRING;
                variantData = (std::string)var;
            }
        }

        /**
         * @fn  void SetObject(const NFGUID& var)
         *
         * @brief   Sets an object.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   var The variable.
         */

        void SetObject(const NFGUID& var)
        {
            if(nType == TDATA_OBJECT || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_OBJECT;
                variantData = (NFGUID)var;
            }
        }

        /**
         * @fn  void SetPoint(const Point3D& var)
         *
         * @brief   Sets a point.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @param   var The variable.
         */

        void SetPoint(const Point3D& var)
        {
            if(nType == TDATA_POINT || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_POINT;
                variantData = (Point3D)var;
            }
        }

        /**
         * @fn  NFINT64 GetInt() const
         *
         * @brief   Gets the int.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  The int.
         */

        NFINT64 GetInt() const
        {
            if(TDATA_INT == nType)
            {
#ifdef HAVE_BOOST
                return boost::get<NFINT64>(variantData);
#else
                return const_cast<variant_type&>(variantData).get<NFINT64>();
#endif
            }

            return NULL_INT;
        }

        /**
         * @fn  double GetDouble() const
         *
         * @brief   Gets the double.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  The double.
         */

        double GetDouble() const
        {
            if(TDATA_DOUBLE == nType)
            {
#ifdef HAVE_BOOST
                return boost::get<double>(variantData);
#else
                return const_cast<variant_type&>(variantData).get<double>();
#endif
            }

            return NULL_DOUBLE;
        }

        /**
         * @fn  const std::string& GetString() const
         *
         * @brief   Gets the string.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  The string.
         */

        const std::string& GetString() const
        {
            if(TDATA_STRING == nType)
            {
#ifdef HAVE_BOOST
                return boost::get<const std::string&>(variantData);
#else
                return const_cast<variant_type&>(variantData).get<std::string>();
#endif
            }

            return NULL_STR;
        }

        /**
         * @fn  const char* GetCharArr() const
         *
         * @brief   Gets character array.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  Null if it fails, else the character array.
         */

        const char* GetCharArr() const
        {
            if(TDATA_STRING == nType)
            {
#ifdef HAVE_BOOST
                return boost::get<const std::string&>(variantData).c_str();
#else
                return const_cast<variant_type&>(variantData).get<std::string>().c_str();
#endif
            }

            return NULL_STR.c_str();
        }

        /**
         * @fn  const NFGUID& GetObject() const
         *
         * @brief   Gets the object.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  The object.
         */

        const NFGUID& GetObject() const
        {
            if(TDATA_OBJECT == nType)
            {
#ifdef HAVE_BOOST
                return boost::get<const NFGUID&>(variantData);
#else
                return const_cast<variant_type&>(variantData).get<NFGUID>();
#endif
            }

            return NULL_GUID;
        }

        /**
         * @fn  const Point3D& GetPoint() const
         *
         * @brief   Gets the point.
         *
         * @author  Nick Yang
         * @date    2016/11/12
         *
         * @return  The point.
         */

        const Point3D& GetPoint() const
        {
            if(TDATA_POINT == nType)
            {
#ifdef HAVE_BOOST
                return boost::get<const Point3D&>(variantData);
#else
                return const_cast<variant_type&>(variantData).get<Point3D>();
#endif
            }

            return NULL_POINT;
        }

        /**
         * @fn  std::string ToString() const
         *
         * @brief   Convert this object into a string representation.
         *
         * @author  Nick Yang
         * @date    2016/11/18
         *
         * @return  A std::string that represents this object.
         */

        std::string ToString() const
        {
            std::string strData;

            switch(nType)
            {
            case TDATA_INT:
                strData = AF_LEXICAL_CAST<std::string>(GetInt());
                break;
            case TDATA_DOUBLE:
                strData = AF_LEXICAL_CAST<std::string>(GetDouble());
                break;
            case TDATA_STRING:
                strData = GetString();
                break;
            case TDATA_OBJECT:
                strData = GetObject().ToString();
                break;
            case TDATA_POINT:
                strData = GetPoint().ToString();
                break;
            default:
                strData = NULL_STR;
                break;
            }
            return strData;
        }

    private:
        /** @brief   The type. */
        TDATA_TYPE nType;

    public:
        /** @brief   Information describing the variant. */
        variant_type variantData;
    };

    /**
     * @fn  AFDataList::AFDataList()
     *
     * @brief   Default constructor.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     */

    explicit AFDataList()
    {
        mnUseSize = 0;
        mvList.reserve(STACK_SIZE);
        for(int i = 0; i < STACK_SIZE; ++i)
        {
            mvList.push_back(NF_NEW TData());
        }
    }

    /**
     * @fn  virtual AFDataList::~AFDataList() = 0;
     *
     * @brief   Destructor.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     */

    virtual ~AFDataList()
    {
        Clear();

        for(int i = 0; i < STACK_SIZE; ++i)
        {
            if(nullptr != mvList[i])
            {
                delete mvList[i];
                mvList[i] = nullptr;
            }
        }
    }

    explicit AFDataList(const std::string& str, const std::string& strSplit)
    {
        Clear();

        Split(str, strSplit);
    }

    explicit AFDataList(const AFDataList& src)
    {
        Clear();

        InnerAppendEx(src, 0, src.GetCount());
    }

    /**
     * @fn  virtual std::string AFDataList::ToString(const int index) const = 0;
     *
     * @brief   Convert this object into a string representation.
     *
     * @author  Nick Yang
     * @date    2016/11/18
     *
     * @param   index   Zero-based index of the TData vector.
     *
     * @return  A std::string that represents this object.
     */

    virtual std::string ToString(const int index) const
    {
        if(ValidIndex(index))
        {
            std::string strData;

            const TDATA_TYPE eType = Type(index);
            switch(eType)
            {
            case TDATA_INT:
                strData = AF_LEXICAL_CAST<std::string>(Int(index));
                break;
            case TDATA_DOUBLE:
                strData = AF_LEXICAL_CAST<std::string>(Double(index));
                break;
            case TDATA_STRING:
                strData = String(index);
                break;
            case TDATA_OBJECT:
                strData = Object(index).ToString();
                break;
            case TDATA_POINT:
                strData = Point(index).ToString();
                break;
            default:
                strData = NULL_STR;
                break;
            }
        }

        return NULL_STR;
    }

    /**
     * @fn  virtual bool AFDataList::ToString(std::string& strSource, const std::string& strSeparator) const = 0;
     *
     * @brief   Convert this object into a string representation with the given separator.
     *
     * @author  Nick Yang
     * @date    2016/11/18
     *
     * @param [in,out]  strSource       The result string.
     * @param           strSeparator    The separator.
     *
     * @return  A bool that represents this object.
     */

    virtual bool ToString(std::string& strOut, const std::string& strSeparator) const
    {
        for(int i = 0; i < GetCount(); ++i)
        {
            std::string strVal = ToString(i);
            strOut += strVal;
            //do not add separator in the last data
            if(i != (GetCount() - 1))
            {
                strOut += strSeparator;
            }
        }

        return true;
    }

public:

    /**
     * @fn  virtual const NF_SHARE_PTR<TData> AFDataList::GetStack(const int index) const = 0;
     *
     * @brief   Gets a stack.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     *
     * @return  The stack.
     */

    virtual TData* GetStack(const int index) const
    {
        if(index < mvList.size())
        {
            return mvList[index];
        }

        return nullptr;
    }

    /**
     * @fn  virtual bool AFDataList::Concat(const AFDataList& src) = 0;
     *
     * @brief   Concatenates the given source into this DataList.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   src     The source AFDataList.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Concat(const AFDataList& src)
    {
        InnerAppendEx(src, 0, src.GetCount());
        return true;
    }

    /**
     * @fn  virtual bool AFDataList::Append(const AFDataList& src) = 0;
     *
     * @brief   Appends a AFDataList to this pointer.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   src The Source to append.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Append(const AFDataList& src)
    {
        return Append(src, 0, src.GetCount());
    }

    /**
     * @fn  virtual bool AFDataList::Append(const AFDataList& src, const int start, const int count) = 0;
     *
     * @brief   Appends a src.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   src     Source for the.
     * @param   start   The start.
     * @param   count   Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Append(const AFDataList& src, const int start, const int count)
    {
        if(start >= src.GetCount())
        {
            return false;
        }

        int end = start + count;

        if(end > src.GetCount())
        {
            return false;
        }

        InnerAppendEx(src, start, end);

        return true;
    }

    /**
     * @fn  virtual bool AFDataList::Append(const AFDataList::TData& sTData) = 0;
     *
     * @brief   Appends a TData into this pointer.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   sTData  The t data to append.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Append(const AFDataList::TData& xData)
    {
        if(xData.GetType() <= TDATA_UNKNOWN
                || xData.GetType() >= TDATA_MAX)
        {
            return false;
        }

        switch(xData.GetType())
        {
        case TDATA_INT:
            AddInt(xData.GetInt());
            break;
        case TDATA_DOUBLE:
            AddDouble(xData.GetDouble());
            break;
        case TDATA_OBJECT:
            AddObject(xData.GetObject());
            break;
        case TDATA_STRING:
            AddString(xData.GetString());
            break;
        case TDATA_POINT:
            AddPoint(xData.GetPoint());
            break;
        default:
            break;
        }

        return true;
    }

    /**
     * @fn  virtual void AFDataList::Clear() = 0;
     *
     * @brief   Clears this object to its blank/initial state.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     */

    virtual void Clear()
    {
        mnUseSize = 0;
        //8个以后的清除掉
        if(mvList.size() > STACK_SIZE)
        {
            for(int i = 0; i < STACK_SIZE; ++i)
            {
                mvList[i]->Reset();
            }

            for(int i = STACK_SIZE; i < mvList.size(); ++i)
            {
                if(nullptr != mvList[i])
                {
                    delete mvList[i];
                    mvList[i] = NULL;
                }
            }

            mvList.erase(mvList.begin() + STACK_SIZE, mvList.end());
        }
    }

    /**
     * @fn  virtual bool AFDataList::IsEmpty() const = 0;
     *
     * @brief   Query if this AFDataList is empty.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @return  True if empty, false if not.
     */

    virtual bool IsEmpty() const
    {
        return (0 == mnUseSize);
    }

    /**
     * @fn  virtual int AFDataList::GetCount() const = 0;
     *
     * @brief   Gets the count.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @return  The count.
     */

    virtual int GetCount() const
    {
        return mnUseSize;
    }

    /**
     * @fn  virtual TDATA_TYPE AFDataList::Type(const int index) const = 0;
     *
     * @brief   Types the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   index   Zero-based index of the.
     *
     * @return  A TDATA_TYPE.
     */

    virtual TDATA_TYPE Type(const int index) const
    {
        if(!ValidIndex(index))
        {
            return TDATA_UNKNOWN;
        }

        if(index < STACK_SIZE)
        {
            return mvList[index]->GetType();
        }
        else
        {
            const TData* pData = GetStack(index);
            if(pData)
            {
                return pData->GetType();
            }
        }

        return TDATA_UNKNOWN;
    }

    /**
     * @fn  virtual bool AFDataList::TypeEx(const int nType, ...) const = 0;
     *
     * @brief   Check the types.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   nType   The type.
     * @param   ...     Variable arguments providing additional information.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool TypeEx(const  int nType, ...) const
    {
        bool bRet = true;

        if(TDATA_UNKNOWN == nType)
        {
            bRet = false;
            return bRet;
        }

        TDATA_TYPE pareType = (TDATA_TYPE)nType;
        va_list arg_ptr;
        va_start(arg_ptr, nType);
        int index = 0;

        while(pareType != TDATA_UNKNOWN)
        {
            //比较
            TDATA_TYPE varType = Type(index);
            if(varType != pareType)
            {
                bRet = false;
                break;
            }

            ++index;
            pareType = (TDATA_TYPE)va_arg(arg_ptr, int);   //获取下一个参数
        }

        va_end(arg_ptr); //结束

        return bRet;
    }

    /**
     * @fn  virtual bool AFDataList::Split(const std::string& str, const std::string& strSplit) = 0;
     *
     * @brief   Splits string with filter into a AFDataList.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   str         The string.
     * @param   strSplit    The split.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Split(const std::string& str, const std::string& strSplit)
    {
        Clear();

        std::string strData(str);
        if(strData.empty())
        {
            return true;
        }

        std::string temstrSplit(strSplit);
        std::string::size_type pos;
        strData += temstrSplit;
        std::string::size_type size = strData.length();

        for(std::string::size_type i = 0; i < size; i++)
        {
            pos = int(strData.find(temstrSplit, i));
            if(pos < size)
            {
                std::string strSub = strData.substr(i, pos - i);
                Add(strSub.c_str());

                i = pos + temstrSplit.size() - 1;
            }
        }

        return true;
    }

    /**
     * @fn  virtual bool AFDataList::Add(const NFINT64 value) = 0;
     *
     * @brief   Adds value.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   value   The value to add.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Add(const NFINT64 value)
    {
        if(GetCount() == mvList.size())
        {
            AddStatck();
        }

        TData* var = GetStack(GetCount());
        if(var)
        {
            var->SetInt(value);
            mnUseSize++;

            return true;
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Add(const double value) = 0;
     *
     * @brief   Adds value.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value to add.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Add(const double value)
    {
        if(GetCount() == mvList.size())
        {
            AddStatck();
        }

        TData* var = GetStack(GetCount());
        if(var)
        {
            var->SetDouble(value);
            mnUseSize++;

            return true;
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Add(const std::string& value) = 0;
     *
     * @brief   Adds value.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value to add.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Add(const std::string& value)
    {
        if(GetCount() == mvList.size())
        {
            AddStatck();
        }

        TData* var = GetStack(GetCount());
        if(var)
        {
            var->SetString(value);
            mnUseSize++;

            return true;
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Add(const NFGUID& value) = 0;
     *
     * @brief   Adds value.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value to add.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Add(const NFGUID& value)
    {
        if(GetCount() == mvList.size())
        {
            AddStatck();
        }

        TData* var = GetStack(GetCount());
        if(var)
        {
            var->SetObject(value);
            mnUseSize++;

            return true;
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Add(const Point3D& value) = 0;
     *
     * @brief   Adds value.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value to add.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Add(const Point3D& value)
    {
        if(GetCount() == mvList.size())
        {
            AddStatck();
        }

        TData* var = GetStack(GetCount());
        if(var)
        {
            var->SetPoint(value);
            mnUseSize++;

            return true;
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Set(const int index, const NFINT64 value) = 0;
     *
     * @brief   Set int64_t value into the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the TData vector.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Set(const int index, const NFINT64 value)
    {
        if(ValidIndex(index) && Type(index) == TDATA_INT)
        {
            TData* var = GetStack(index);
            if(var)
            {
                var->SetInt(value);

                return true;
            }
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Set(const int index, const double value) = 0;
     *
     * @brief   Set double value into the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the TData vector.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Set(const int index, const double value)
    {
        if(ValidIndex(index) && Type(index) == TDATA_DOUBLE)
        {
            TData* var = GetStack(index);
            if(var)
            {
                var->SetDouble(value);

                return true;
            }
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Set(const int index, const std::string& value) = 0;
     *
     * @brief   Set std::string value into the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the TData vector.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Set(const int index, const std::string& value)
    {
        if(ValidIndex(index) && Type(index) == TDATA_STRING)
        {
            TData* var = GetStack(index);
            if(var)
            {
                var->SetString(value);

                return true;
            }
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Set(const int index, const NFGUID& value) = 0;
     *
     * @brief   Set NFGUID value into the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the TData vector.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Set(const int index, const NFGUID& value)
    {
        if(ValidIndex(index) && Type(index) == TDATA_OBJECT)
        {
            TData* var = GetStack(index);
            if(var)
            {
                var->SetObject(value);

                return true;
            }
        }

        return false;
    }

    /**
     * @fn  virtual bool AFDataList::Set(const int index, const Point3D& value) = 0;
     *
     * @brief   Set Point3D value into the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the TData vector.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Set(const int index, const Point3D& value)
    {
        if(ValidIndex(index) && Type(index) == TDATA_POINT)
        {
            TData* var = GetStack(index);
            if(var)
            {
                var->SetPoint(value);

                return true;
            }
        }

        return false;
    }

    /**
     * @fn  virtual NFINT64 AFDataList::Int(const int index) const = 0;
     *
     * @brief   Get Int value of the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/16
     *
     * @param   index   Zero-based index of the TData vector.
     *
     * @return  A NFINT64.
     */

    virtual NFINT64 Int(const int index) const
    {
        if(ValidIndex(index))
        {
            if(Type(index) == TDATA_INT)
            {
                const TData* var = GetStack(index);
                return var->GetInt();
            }
        }

        return NULL_INT;
    }

    /**
     * @fn  virtual double AFDataList::Double(const int index) const = 0;
     *
     * @brief   Doubles the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     *
     * @return  A double.
     */

    virtual double Double(const int index) const
    {
        if(ValidIndex(index))
        {
            const TData* var = mvList[index];
            if(var && TDATA_DOUBLE == var->GetType())
            {
                return var->GetDouble();
            }
        }

        return NULL_DOUBLE;
    }

    /**
     * @fn  virtual const std::string& AFDataList::String(const int index) const = 0;
     *
     * @brief   Strings the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     *
     * @return  A reference to a const std::string.
     */

    virtual const std::string& String(const int index) const
    {
        if(ValidIndex(index))
        {
            const TData* var = mvList[index];
            if(var && TDATA_STRING == var->GetType())
            {
                return var->GetString();
            }
        }

        return NULL_STR;
    }

    /**
     * @fn  virtual const NFGUID& AFDataList::Object(const int index) const = 0;
     *
     * @brief   Objects the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     *
     * @return  A reference to a const NFGUID.
     */

    virtual const NFGUID& Object(const int index) const
    {
        if(ValidIndex(index))
        {
            TDATA_TYPE type = Type(index);
            if(TDATA_OBJECT == type)
            {
                TData* var = GetStack(index);
                if(nullptr != var)
                {
                    return var->GetObject();
                }
            }
        }

        return NULL_GUID;
    }

    /**
     * @fn  virtual const Point3D& AFDataList::Point(const int index) const = 0;
     *
     * @brief   Points the given index.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     *
     * @return  A reference to a const Point3D.
     */

    virtual const Point3D& Point(const int index) const
    {
        if(ValidIndex(index))
        {
            TDATA_TYPE type = Type(index);
            if(TDATA_OBJECT == type)
            {
                TData* var = GetStack(index);
                if(nullptr != var)
                {
                    return var->GetPoint();
                }
            }
        }

        return NULL_POINT;
    }

    /**
     * @fn  bool AFDataList::AddInt(const NFINT64 value)
     *
     * @brief   Adds an int.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool AddInt(const NFINT64 value)
    {
        return Add(value);
    }

    /**
     * @fn  bool AFDataList::AddDouble(const double value)
     *
     * @brief   Adds a double.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool AddDouble(const double value)
    {
        return Add(value);
    }

    /**
     * @fn  bool AFDataList::AddString(const std::string& value)
     *
     * @brief   Adds a string.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool AddString(const std::string& value)
    {
        return Add(value);
    }

    /**
     * @fn  bool AFDataList::AddStringFromChar(const char* value)
     *
     * @brief   Adds a string from character.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool AddStringFromChar(const char* value)
    {
        return Add(value);
    }

    /**
     * @fn  bool AFDataList::AddObject(const NFGUID& value)
     *
     * @brief   Adds an object.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool AddObject(const NFGUID& value)
    {
        return Add(value);
    }

    /**
     * @fn  bool AFDataList::AddPoint(const Point3D& value)
     *
     * @brief   Adds a point.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool AddPoint(const Point3D& value)
    {
        return Add(value);
    }

    /**
     * @fn  bool AFDataList::SetInt(const int index, const NFINT64 value)
     *
     * @brief   Sets an int.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool SetInt(const int index, const NFINT64 value)
    {
        return Set(index, value);
    }

    /**
     * @fn  bool AFDataList::SetFloat(const int index, const double value)
     *
     * @brief   Sets a float.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool SetFloat(const int index, const double value)
    {
        return Set(index, value);
    }

    /**
     * @fn  bool AFDataList::SetString(const int index, const std::string& value)
     *
     * @brief   Sets a string.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool SetString(const int index, const std::string& value)
    {
        return Set(index, value);
    }

    /**
     * @fn  bool AFDataList::SetObject(const int index, const NFGUID& value)
     *
     * @brief   Sets an object.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool SetObject(const int index, const NFGUID& value)
    {
        return Set(index, value);
    }

    /**
     * @fn  bool AFDataList::SetPoint(const int index, const Point3D& value)
     *
     * @brief   Sets a point.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   index   Zero-based index of the.
     * @param   value   The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool SetPoint(const int index, const Point3D& value)
    {
        return Set(index, value);
    }

    /**
     * @fn  inline bool AFDataList::Compare(const int nPos, const AFDataList& src) const
     *
     * @brief   Compares two const int objects to determine their relative ordering.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   nPos    Constant int to be compared.
     * @param   src     Constant nfi data list&amp; to be compared.
     *
     * @return  True if it succeeds, false if it fails.
     */

    inline bool Compare(const int nPos, const AFDataList& src) const
    {
        if(src.GetCount() > nPos
                && GetCount() > nPos
                && src.Type(nPos) == Type(nPos))
        {
            switch(src.Type(nPos))
            {
            case TDATA_INT:
                return Int(nPos) == src.Int(nPos);
                break;
            case TDATA_DOUBLE:
                return IsZeroDouble(Double(nPos) - src.Double(nPos));
                break;
            case TDATA_STRING:
                return String(nPos) == src.String(nPos);
                break;
            case TDATA_OBJECT:
                return Object(nPos) == src.Object(nPos);
                break;
            case TDATA_POINT:
                return Point(nPos) == src.Point(nPos);
                break;
            default:
                return false;
                break;
            }
        }

        return false;
    }

    /**
     * @fn  inline bool AFDataList::operator==(const AFDataList& src) const
     *
     * @brief   Equality operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   src Source for the.
     *
     * @return  True if the parameters are considered equivalent.
     */

    inline bool operator==(const AFDataList& src) const
    {
        if(src.GetCount() == GetCount())
        {
            for(int i = 0; i < GetCount(); i++)
            {
                if(!Compare(i, src))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    /**
     * @fn  inline bool AFDataList::operator!=(const AFDataList& src)
     *
     * @brief   Inequality operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   src Source for the.
     *
     * @return  True if the parameters are not considered equivalent.
     */

    inline bool operator!=(const AFDataList& src)
    {
        return !(*this == src);
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const double value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const double value)
    {
        Add(value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const char* value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const char* value)
    {
        Add(value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const std::string& value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const std::string& value)
    {
        Add(value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const NFINT64& value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const NFINT64& value)
    {
        Add(value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const int value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const int value)
    {
        Add((NFINT64)value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const NFGUID& value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const NFGUID& value)
    {
        Add(value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const Point3D& value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const Point3D& value)
    {
        Add(value);
        return *this;
    }

    /**
     * @fn  inline AFDataList& AFDataList::operator<<(const AFDataList& value)
     *
     * @brief   Bitwise left shift operator.
     *
     * @author  Nick Yang
     * @date    2016/11/12
     *
     * @param   value   The value.
     *
     * @return  The shifted result.
     */

    inline AFDataList& operator<<(const AFDataList& value)
    {
        Concat(value);
        return *this;
    }


    AFDataList& operator=(const AFDataList& xData)
    {
        if(this != &xData)
        {
            this->Clear();
            Append(xData);
        }

        return *this;
    }

    /**
     * @enum
     *
     * @brief   Values that represent s.
     */

    enum { STACK_SIZE = 8 };

protected:
    void AddStatck()
    {
        for(int i = 0; i < STACK_SIZE; ++i)
        {
            mvList.push_back(NF_NEW TData());
        }
    }

    bool ValidIndex(int index) const
    {
        return (index < GetCount()) && (index >= 0);
    }

    void InnerAppendEx(const AFDataList& src, const int start, const int end)
    {
        for(int i = start; i < end; ++i)
        {
            TDATA_TYPE vType = src.Type(i);
            switch(vType)
            {
            case TDATA_INT:
                AddInt(src.Int(i));
                break;
            case TDATA_DOUBLE:
                AddDouble(src.Double(i));
                break;
            case TDATA_STRING:
                AddString(src.String(i));
                break;
            case TDATA_OBJECT:
                AddObject(src.Object(i));
                break;
            case TDATA_POINT:
                AddPoint(src.Point(i));
                break;
            default:
                break;
            }
        }
    }


protected:
    /** @brief   Size of the mn use. */
    int mnUseSize;
    /** @brief   List of mvs. */
    std::vector< TData*> mvList;
};

/** @brief   The null tdata. */
const static AFDataList::TData NULL_TDATA = AFDataList::TData();

#endif