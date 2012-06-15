
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnfobj.hpp>
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

  ase_variant operator ()(ASE_Component::Library& lib,
    const ase_variant *args, ase_size_type nargs) {

    Var Java = lib.GetGlobals().get("Java");
    Var JFrame = Java.get("javax.swing.JFrame");
    Var JPanel = Java.get("javax.swing.JPanel");
    Var JTextArea = Java.get("javax.swing.JTextArea");
    Var JTextField = Java.get("javax.swing.JTextField");
    Var JLabel = Java.get("javax.swing.JLabel");
    Var JButton = Java.get("javax.swing.JButton");
    Var JScrollPane = Java.get("javax.swing.JScrollPane");
    Var EmptyBorder = Java.get("javax.swing.border.EmptyBorder");
    Var Dimension = Java.get("java.awt.Dimension");
    Var GridBagLayout = Java.get("java.awt.GridBagLayout");
    Var GridBagConstraints = Java.get("java.awt.GridBagConstraints");
    Var Insets = Java.get("java.awt.Insets");
    Var ActionListener = Java.get("java.awt.event.ActionListener");

    Var fr = JFrame.invoke();
    fr.minvoke("setBounds", 10, 10, 550, 250);
    Var p = JPanel.invoke();
    Var layout = GridBagLayout.invoke();
    p.minvoke("setLayout", layout);
    fr.minvoke("getContentPane").minvoke("add", p);
    p.minvoke("setBorder", EmptyBorder.invoke(5, 5, 5, 5));
    fr.minvoke("setMinimumSize", Dimension.invoke(300, 150));

    Var la = JLabel.invoke("Expression");
    Var tf = JTextArea.invoke("", 20, 30);
    Var sp = JScrollPane.invoke(tf);
    Var la2 = JLabel.invoke("value");
    Var tf2 = JTextField.invoke("", 20);
    tf2.minvoke("setEditable", false);
    Var lnr = ase_new_string_map();
    lnr.set("actionPerformed",
      ase_new_function_object(click_cb(lib.GetGlobals(), tf, tf2)));

    Var bt = JButton.invoke("Evaluate");
    bt.minvoke("addActionListener", ActionListener.invoke(lnr));

    Var gbc = GridBagConstraints.invoke();
    gbc.set("insets", Insets.invoke(2, 2, 2, 2));

    gbc.set("gridx", 0);
    gbc.set("gridy",  0);
    gbc.set("gridwidth", 1);
    gbc.set("gridheight", 1);
    gbc.set("weightx", 0.0);
    gbc.set("weighty", 1.0);
    gbc.set("fill", GridBagConstraints.get("VERTICAL"));
    layout.minvoke("setConstraints", la, gbc);
    p.minvoke("add", la);

    gbc.set("gridx", 1);
    gbc.set("gridy", 0);
    gbc.set("gridwidth", 1);
    gbc.set("gridheight", 1);
    gbc.set("weightx", 1.0);
    gbc.set("weighty", 1.0);
    gbc.set("fill", GridBagConstraints.get("BOTH"));
    layout.minvoke("setConstraints", sp, gbc);
    p.minvoke("add", sp);

    gbc.set("gridx", 2);
    gbc.set("gridy", 0);
    gbc.set("gridwidth", 1);
    gbc.set("gridheight", 2);
    gbc.set("weightx", 0.0);
    gbc.set("weighty", 1.0);
    gbc.set("fill", GridBagConstraints.get("VERTICAL"));
    layout.minvoke("setConstraints", bt, gbc);
    p.minvoke("add", bt);

    gbc.set("gridx", 0);
    gbc.set("gridy", 1);
    gbc.set("gridwidth", 1);
    gbc.set("gridheight", 1);
    gbc.set("weightx", 0.0);
    gbc.set("weighty", 0.0);
    gbc.set("fill", GridBagConstraints.get("VERTICAL"));
    layout.minvoke("setConstraints", la2, gbc);
    p.minvoke("add", la2);

    gbc.set("gridx", 1);
    gbc.set("gridy", 1);
    gbc.set("gridwidth", 1);
    gbc.set("gridheight", 1);
    gbc.set("weightx", 0.0);
    gbc.set("weighty", 0.0);
    gbc.set("fill", GridBagConstraints.get("HORIZONTAL"));
    layout.minvoke("setConstraints", tf2, gbc);
    p.minvoke("add", tf2);

    fr.minvoke("setDefaultCloseOperation", JFrame.get("EXIT_ON_CLOSE"));
    fr.minvoke("show");

    return ase_variant();
  }

}; 

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  ASE_ComponentFuncObject::NewLibraryFactory<swingtest, false>(ap_r);
}

