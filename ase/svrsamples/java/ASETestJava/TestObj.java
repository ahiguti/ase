
package ASETestJava;

public class TestObj {

  public TestObj() {
    intval = 0;
  }
  public void SetInt(int x) {
    intval = x;
  }
  public int GetInt() {
    return intval;
  }
  public void AddInt(int x) {
    intval += x;
  }
  public void SetString(String x) {
    strval = x;
  }
  public String getString() {
    return strval;
  }
  public void AddStrlen(String x) {
    intval += x.length();
  }
  public void SetCallback(CallbackInterface cb) {
    callback = cb;
  }
  public int CallbackInt(int count, int x, int y, int z) {
    int r = 0;
    System.out.println("CallbackInt x=" + x + " y=" + y + " z=" + z);
    for (int i = 0; i < count; ++i) {
      r = callback.CallInt(x, y, z);
    }
    System.out.println("CallbackInt r=" + r);
    return r;
  }
  public String CallbackString(int count, String x, String y, String z) {
    String r = null;
    System.out.println("CallbackString x=" + x + " y=" + y + " z=" + z);
    for (int i = 0; i < count; ++i) {
      r = callback.CallString(x, y, z);
    }
    System.out.println("CallbackString r=" + r);
    return r;
  }

  private int intval;
  private String strval;
  private CallbackInterface callback;

};

