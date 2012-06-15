using System;
using System.Collections.Generic;
using System.Text;

namespace ASETestCLR
{
    public delegate Object CallbackDelegate(Object x, Object y, Object z);

    public class TestObj
    {
        public void SetInt(int x)
        {
            intval = x;
        }
        public int GetInt()
        {
            return intval;
        }
        public void AddInt(int x)
        {
            intval += x;
        }
        public void SetString(String x)
        {
            strval = x;
        }
        public String GetString()
        {
            return strval;
        }
        public void AddStrlen(String x)
        {
            intval += x.Length;
        }
        public void SetCallback(CallbackDelegate x)
        {
            callback = x;
        }
        public int CallbackInt(int count, int x, int y, int z)
        {
            Object[] args = new Object[3] { x, y, z };
            Object r = null;
            for (int i = 0; i < count; ++i)
            {
                r = callback(x, y, z);
            }
            return (int)r;
        }
        public String CallbackString(int count, String x, String y, String z)
        {
            Object[] args = new Object[3] { x, y, z };
            Object r = null;
            for (int i = 0; i < count; ++i)
            {
                r = callback(x, y, z);
            }
            return r.ToString();
        }

        int intval;
        String strval;
        CallbackDelegate callback;
    }
}
