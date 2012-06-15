
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <cstdio>
#include <cstdlib>

#include <jni.h>

int main()
{
  JavaVM *jvm = NULL;
  JNIEnv *env = NULL;
  JavaVMInitArgs vm_args;
  vm_args.version = 0x00010002;
  vm_args.nOptions = 0;
  vm_args.options = NULL;
  vm_args.ignoreUnrecognized = JNI_FALSE;
  JNI_GetDefaultJavaVMInitArgs(&vm_args);
  JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);
  if (!env) {
    std::fprintf(stderr, "env is null\n");
    exit(-1);
  }
  jclass cls = env->FindClass("TestClass");
  if (!cls) {
    std::fprintf(stderr, "cls is null\n");
    exit(-1);
  }
  jmethodID mctr = env->GetMethodId(cls, "<init>", "()V");
  if (!mctr) {
    std::fprintf(stderr, "mctr is null\n");
    exit(-1);
  }
  jobject jo = env->NewObject(cls, mctr); 
  jmethodID mid = env->GetMethodId(cls, "foo", "(I)I");
  if (!mid) {
    std::fprintf(stderr, "mid is null\n");
    exit(-1);
  }
  int rv = env->CallIntMethod(jo, mid, 150);
  std::fprintf(stderr, "rv=%d\n", rv);
  jmethodID mbar = env->GetMethodId(cls, "bar", "()I");
  if (!mbar) {
    std::fprintf(stderr, "mbar is null\n");
    exit(-1);
  }
  int x = 0;
  for (int i = 0; i < 10000000; ++i) {
    x = env->CallIntMethod(jo, mbar);
  }
  std::fprintf(stderr, "x=%d\n", x);
  jvm->DestroyJavaVM();
}

