
var sb = Java["java.lang.StringBuilder"].NewInstance();
sb.append("abc");
sb.append(35.9);
sb.append("def");
var s = sb.toString();
print(s);

