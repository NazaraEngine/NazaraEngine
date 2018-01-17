Examples writing-style:

Alphabetical order for everything and try to regroup each methods beginning with the same letter in the header

Class header:
```cpp
// Copyright (C) YEAR AUTHOR
// This file is part of the "Nazara Engine - MODULE module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILENAME_HPP
#define NAZARA_FILENAME_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/NzClass.hpp>
#include <STLHeader>

struct NzPossibleImplementation;

class NAZARA_API NzClassName
{
	friend NzClassFriend;

	public:
		Constructors();
		Destructor();

		FunctionInAClass();

	protected:

	private:
		NzClass m_variableName;
		STL m_variableName;
		dataType m_variableName;

		NzPossibleImplementation* m_impl;
};

#endif // NAZARA_FILENAME_HPP
```

Class source:
```cpp
// Copyright (C) YEAR AUTHOR
// This file is part of the "Nazara Engine - MODULE module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/MODULE/FILENAME.hpp>
#include <Nazara/NzClass.hpp>
#include <STLHeader>
#include <Nazara/MODULE/Debug.hpp>

struct NzPossibleImplementation {};

NzClassName::Constructors() :
m_variableName(init)
{
	testsAndOtherInits;
}

NzClassName::PublicFunctions()
NzClassName::ProtectedFunctions()
NzClassName::PrivateFunctions()
```

Structure:
```cpp
/!\ enum in Enums.hpp

enum nzEnum
{
	nzEnum_1,
	nzEnum_2,

	nzEnum_Max = nzEnum_2
};
```

Function:
```cpp
FunctionName()
{
	variableName = init;
}
```
