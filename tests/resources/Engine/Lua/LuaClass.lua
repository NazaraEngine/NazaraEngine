
function test_Test()
    local test = Test(1)

    local i = test:GetI()
    local result = Test.StaticMethodWithArguments(i, i)
    local finalTest = Test(result + test:GetDefault(), true)

    CheckTest(test)
    CheckStatic(result)
    CheckFinalTest(finalTest)
end

function test_InheritTest()
    local test = InheritTest()

    CheckInheritTest(test)
end

function test_TestHandle()
    local test = TestHandle()

    CheckTestHandle()
end
