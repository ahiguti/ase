
// package asejni;

import java.util.Arrays;
import java.io.File;
import javax.tools.*;

public class Compiler {

  public static void compile(String filename) {
    JavaCompiler compiler = ToolProvider.getSystemJavaCompiler();
    StandardJavaFileManager fm = compiler.getStandardFileManager(null, null,
      null);
    Iterable<? extends JavaFileObject> jfs = fm.getJavaFileObjectsFromFiles(
      Arrays.asList(new File[] { new File(filename) }));
    compiler.getTask(null, fm, null, null, null, jfs).call();
  }

  public static void main(String[] args) {
    compile(args[0]);
  }

}

