
function fib(x)
  if x <= 1 then
    return 1
  else
    return fib(x - 2) + fib(x - 1)
  end
end

print(fib(30))
