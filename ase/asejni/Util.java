
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

package asejni;

import java.io.*;

class Util {

  private static final ClassLoader currentDirClassLoader;

  static {
    File f = new File(".");
    ClassLoader ldr = null;
    try {
      java.net.URL url = f.getCanonicalFile().toURI().toURL();
      ldr = new java.net.URLClassLoader(new java.net.URL[] { url });
    } catch (java.net.MalformedURLException e) {
      /* ignore */
    } catch (java.io.IOException e) {
      /* ignore */
    }
    currentDirClassLoader = ldr;
  }

  public static String objectToString(Object obj) {
    if (obj instanceof Throwable) {
      Throwable exc = (Throwable)obj;
      StringWriter sw = new StringWriter();
      PrintWriter pw = new PrintWriter(sw);
      exc.printStackTrace(pw);
      return "Java: " + sw.toString();
    }
    return obj.toString();
  }

  public static int joinAll() {
    Thread cur = Thread.currentThread();
    ThreadGroup gr = cur.getThreadGroup();
    Thread tarr[] = new Thread[10];
    while (true) {
      int n = gr.enumerate(tarr);
      if (n >= tarr.length) {
	tarr = new Thread[tarr.length + 1];
	continue;
      }
      int i = 0;
      for (i = 0; i < n; ++i) {
	Thread t = tarr[i];
	if (t != cur && !t.isDaemon()) {
	  break;
	}
      }
      if (i == n) {
	break;
      }
      try {
	tarr[i].join();
      } catch (Throwable ex) {
	/* ignore */
      }
    }
    return 0;
  }

  public static Class loadClass(String name, ClassLoader ldr) {
    try {
      return Class.forName(name, true, ldr);
    } catch (ClassNotFoundException e) {
      return null;
    }
  }

  public static Class loadClassCurrentDir(String name) {
    try {
      return Class.forName(name, true, currentDirClassLoader);
    } catch (ClassNotFoundException e) {
      return null;
    }
  }

}

