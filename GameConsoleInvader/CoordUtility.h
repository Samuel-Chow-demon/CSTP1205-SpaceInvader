#pragma once

struct st2D
{
    st2D(int v1 = 0, int v2 = 0) :
        val1(v1), val2(v2)
    {}
    int val1, val2;
};

struct st2DRange : public st2D
{
    st2DRange(int iLow, int iHigh) :
        st2D(iLow, iHigh)
    {}

    int GetLowerBound(void) const
    {
        return val1;
    }
    int GetHigherBound(void) const
    {
        return val2;
    }
    bool IsValid(void) const
    {
        return val1 != val2;
    }
};

struct st2DXY : public st2D
{
    st2DXY(const st2D& stPos) :
        st2D(stPos), m_stXLmt{ st2DRange(0, 0) }, m_stYLmt{ st2DRange(0,0) }
    {}
    st2DXY(int x = 0, int y = 0) :
        st2D(x, y), m_stXLmt{ st2DRange(0, 0) }, m_stYLmt{ st2DRange(0,0) }
    {}
    static void ChkAndLmtToBoundary(int& val, const st2DRange& stLmt)
    {
        if (stLmt.IsValid())
        {
            const int iLowLmt = stLmt.GetLowerBound();
            const int iHighLmt = stLmt.GetHigherBound();

            if (val < iLowLmt)
            {
                val = iLowLmt;
            }

            if (val > iHighLmt)
            {
                val = iHighLmt;
            }
        }
    }

    void SetXLmt(const st2DRange& stLmt)
    {
        m_stXLmt = stLmt;
    }
    void SetYLmt(const st2DRange& stLmt)
    {
        m_stYLmt = stLmt;
    }

    void XMove(int iMoveStep)
    {
        val1 += iMoveStep;
        ChkAndLmtToBoundary(val1, m_stXLmt);
    }
    void YMove(int iMoveStep)
    {
        val2 += iMoveStep;
        ChkAndLmtToBoundary(val2, m_stYLmt);
    }
    bool IsOutOfBoundary(void)
    {
        return val1 <= m_stXLmt.GetLowerBound() || val1 >= m_stXLmt.GetHigherBound() ||
                val2 <= m_stYLmt.GetLowerBound() || val2 >= m_stYLmt.GetHigherBound();
    }

    st2DRange GetXLmt() const
    {
        return m_stXLmt;
    }
    st2DRange GetYLmt() const
    {
        return m_stYLmt;
    }

    int X() const
    {
        return val1;
    }
    int Y() const
    {
        return val2;
    }
private:
    st2DRange m_stXLmt, m_stYLmt;
};