
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecclass.hpp>
#include <ase/asestrmap.hpp>
#include <ase/asefuncobj.hpp>

namespace {

struct swingtest {

  typedef ase_variant Var;

  struct click_cb {
    click_cb(const Var& gl, const Var& tf, const Var& tf2)
      : globals(gl), txfld(tf), txfld2(tf2) { }
    Var operator ()(const Var *args, ase_size_type nargs) {
      Var txt = txfld.minvoke("getText");
      txfld2.minvoke("setText", txt);
      std::cout << "click_cb called: " << txt << std::endl;
      return Var();
    }
    Var globals, txfld, txfld2;
  };

  struct add_control {
    add_control(const Var& gk, const Var& p, const Var& l, const Var& g)
      : gbcklass(gk), pnl(p), layout(l), gbc(g) { }
    void operator ()(const Var& c, int gx, int gy, int gw, int gh,
      double wx, double wy, const char *fill) {
      gbc.set("gridx", gx);
      gbc.set("gridy", gy);
      gbc.set("gridwidth", gw);
      gbc.set("gridheight", gh);
      gbc.set("weightx", wx);
      gbc.set("weighty", wy);
      gbc.set("fill", gbcklass.get_property(ase_string(fill)));
      layout.minvoke("setConstraints", c, gbc);
      pnl.minvoke("add", c);
    }
    Var gbcklass, pnl, layout, gbc;
  };

  static ase_variant Main(const ase_variant& globalobj) {

    printf("[%s]\n", globalobj.get_string().data());
    Var ASE = globalobj.get("ASE");
    Var Java = ASE.get("Java");
    Var Swing = Java.get("javax.swing");
    Var AWT = Java.get("java.awt");

    Var fr = Swing.minvoke("JFrame");
    fr.minvoke("setBounds", 10, 10, 550, 250);
    Var p = Swing.minvoke("JPanel");
    Var layout = AWT.minvoke("GridBagLayout");
    p.minvoke("setLayout", layout);
    fr.minvoke("getContentPane").minvoke("add", p);
    p.minvoke("setBorder",
      Swing.get("border").minvoke("EmptyBorder", 5, 5, 5, 5));
    fr.minvoke("setMinimumSize", AWT.minvoke("Dimension", 300, 150));

    Var la = Swing.minvoke("JLabel", "Expression");
    Var tf = Swing.minvoke("JTextArea", "", 20, 30);
    Var sp = Swing.minvoke("JScrollPane", tf);
    Var la2 = Swing.minvoke("JLabel", "value");
    Var tf2 = Swing.minvoke("JTextField", "", 20);
    tf2.minvoke("setEditable", false);
    Var lnr = ase_new_string_map();
    lnr.set("actionPerformed",
      ase_new_function_object(click_cb(globalobj, tf, tf2)));

    Var bt = Swing.minvoke("JButton", "Evaluate");
    bt.minvoke("addActionListener",
      AWT.get("event").minvoke("ActionListener", lnr));

    Var gbc = AWT.minvoke("GridBagConstraints");
    gbc.set("insets", AWT.minvoke("Insets", 2, 2, 2, 2));

    add_control addctl(AWT.get("GridBagConstraints"), p, layout, gbc);
    addctl(la, 0, 0, 1, 1, 0.0, 1.0, "VERTICAL");
    addctl(sp, 1, 0, 1, 1, 1.0, 1.0, "BOTH");
    addctl(bt, 2, 0, 1, 2, 0.0, 1.0, "VERTICAL");
    addctl(la2, 0, 1, 1, 1, 0.0, 0.0, "VERTICAL");
    addctl(tf2, 1, 1, 1, 1, 0.0, 0.0, "HORIZONTAL");

    fr.minvoke("setDefaultCloseOperation",
      Swing.get("JFrame").get("EXIT_ON_CLOSE"));
    fr.minvoke("show");
    return ase_variant();
  }

}; 

}; // anonymous namespace

extern "C" ase_variant
ASE_DLLMain(const ase_variant& gl)
{
  ase_cclass<swingtest>::initializer()
    .def("Main", &swingtest::Main)
    ;
  const ase_variant kl = ase_cclass<swingtest>::get_class();
  const ase_variant r = kl.minvoke("Main", gl);
  return r;
  #if 0
  return swingtest::Main(gl);
  #endif
}

