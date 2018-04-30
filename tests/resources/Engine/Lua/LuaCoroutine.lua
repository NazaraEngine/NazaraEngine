
function even (x)
    coroutine.yield(1)
end

function odd (x)
    coroutine.yield(0)
end

function infinite (x)
    for i=1,x do
        if i==3 then coroutine.yield(-1) end
        if i % 2 == 0 then even(i) else odd(i) end
    end
end
