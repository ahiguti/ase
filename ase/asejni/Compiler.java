
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* JAVA2_IMPL */

package asejni;

import java.util.Arrays;
import java.io.File;
import javax.tools.*;

public class Compiler {

  public static void compile(String filename) {
    JavaCompiler compiler = ToolProvider.getSystemJavaCompiler();
    if (compiler == null) {
      throw new java.lang.RuntimeException("Compiler not found");
    }
    StandardJavaFileManager fm = compiler.getStandardFileManager(null, null,
      null);
    Iterable<? extends JavaFileObject> jfs = fm.getJavaFileObjectsFromFiles(
      Arrays.asList(new File[] { new File(filename) }));
    compiler.getTask(null, fm, null, null, null, jfs).call();
  }

}

