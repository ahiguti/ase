
Perl.Use('Term::Cap')
local t = Perl.Methods['Term::Cap'].Tgetent()
t.Trequire('cl', 'cm')
local s = ""
s = s .. t.Tputs('cl', 1)
s = s .. t.Tgoto('cm', 20, 5)
print(s .. '<- here is (20, 5)')

