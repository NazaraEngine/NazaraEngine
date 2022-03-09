// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/AstConstantPropagationVisitor.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	namespace
	{
		template<typename T, typename U>
		std::unique_ptr<T> static_unique_pointer_cast(std::unique_ptr<U>&& ptr)
		{
			return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
		}

		template <typename T>
		struct is_complete_helper
		{
			// SFINAE: sizeof in an incomplete type is an error, but since there's another specialization it won't result in a compilation error
			template <typename U>
			static auto test(U*) -> std::bool_constant<sizeof(U) == sizeof(U)>;

			// less specialized overload
			static auto test(...) -> std::false_type;

			using type = decltype(test(static_cast<T*>(nullptr)));
		};

		template <typename T>
		struct is_complete : is_complete_helper<T>::type {};

		template<typename T>
		inline constexpr bool is_complete_v = is_complete<T>::value;

		template<typename T>
		struct VectorInfo
		{
			static constexpr std::size_t Dimensions = 1;
			using Base = T;
		};

		template<typename T>
		struct VectorInfo<Vector2<T>>
		{
			static constexpr std::size_t Dimensions = 2;
			using Base = T;
		};

		template<typename T>
		struct VectorInfo<Vector3<T>>
		{
			static constexpr std::size_t Dimensions = 3;
			using Base = T;
		};

		template<typename T>
		struct VectorInfo<Vector4<T>>
		{
			static constexpr std::size_t Dimensions = 4;
			using Base = T;
		};

		/*************************************************************************************************/

		template<BinaryType Type, typename T1, typename T2>
		struct BinaryConstantPropagation;

		// CompEq
		template<typename T1, typename T2>
		struct BinaryCompEqBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs == rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompEq;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompEq, T1, T2>
		{
			using Op = BinaryCompEq<T1, T2>;
		};

		// CompGe
		template<typename T1, typename T2>
		struct BinaryCompGeBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs >= rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompGe;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompGe, T1, T2>
		{
			using Op = BinaryCompGe<T1, T2>;
		};

		// CompGt
		template<typename T1, typename T2>
		struct BinaryCompGtBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs > rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompGt;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompGt, T1, T2>
		{
			using Op = BinaryCompGt<T1, T2>;
		};

		// CompLe
		template<typename T1, typename T2>
		struct BinaryCompLeBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs <= rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompLe;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompLe, T1, T2>
		{
			using Op = BinaryCompLe<T1, T2>;
		};

		// CompLt
		template<typename T1, typename T2>
		struct BinaryCompLtBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs < rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompLt;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompLt, T1, T2>
		{
			using Op = BinaryCompLe<T1, T2>;
		};

		// CompNe
		template<typename T1, typename T2>
		struct BinaryCompNeBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs != rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryCompNe;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::CompNe, T1, T2>
		{
			using Op = BinaryCompNe<T1, T2>;
		};

		// LogicalAnd
		template<typename T1, typename T2>
		struct BinaryLogicalAndBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs && rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryLogicalAnd;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::LogicalAnd, T1, T2>
		{
			using Op = BinaryLogicalAnd<T1, T2>;
		};

		// LogicalOr
		template<typename T1, typename T2>
		struct BinaryLogicalOrBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs || rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryLogicalOr;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::LogicalOr, T1, T2>
		{
			using Op = BinaryLogicalOr<T1, T2>;
		};

		// Addition
		template<typename T1, typename T2>
		struct BinaryAdditionBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs + rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryAddition;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Add, T1, T2>
		{
			using Op = BinaryAddition<T1, T2>;
		};

		// Division
		template<typename T1, typename T2>
		struct BinaryDivisionBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs / rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryDivision;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Divide, T1, T2>
		{
			using Op = BinaryDivision<T1, T2>;
		};

		// Multiplication
		template<typename T1, typename T2>
		struct BinaryMultiplicationBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs * rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinaryMultiplication;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Multiply, T1, T2>
		{
			using Op = BinaryMultiplication<T1, T2>;
		};

		// Subtraction
		template<typename T1, typename T2>
		struct BinarySubtractionBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T1& lhs, const T2& rhs)
			{
				return ShaderBuilder::Constant(lhs - rhs);
			}
		};

		template<typename T1, typename T2>
		struct BinarySubtraction;

		template<typename T1, typename T2>
		struct BinaryConstantPropagation<BinaryType::Subtract, T1, T2>
		{
			using Op = BinarySubtraction<T1, T2>;
		};

		/*************************************************************************************************/

		template<typename T, typename... Args>
		struct CastConstantBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const Args&... args)
			{
				return ShaderBuilder::Constant(T(args...));
			}
		};

		template<typename T, typename... Args>
		struct CastConstant;

		/*************************************************************************************************/

		template<typename T, std::size_t TargetComponentCount, std::size_t FromComponentCount>
		struct SwizzlePropagationBase;

		template<typename T, std::size_t TargetComponentCount>
		struct SwizzlePropagationBase<T, TargetComponentCount, 1>
		{
			std::unique_ptr<ConstantValueExpression> operator()(const std::array<UInt32, 4>& /*components*/, T value)
			{
				if constexpr (TargetComponentCount == 4)
					return ShaderBuilder::Constant(Vector4<T>(value, value, value, value));
				else if constexpr (TargetComponentCount == 3)
					return ShaderBuilder::Constant(Vector3<T>(value, value, value));
				else if constexpr (TargetComponentCount == 2)
					return ShaderBuilder::Constant(Vector2<T>(value, value));
				else if constexpr (TargetComponentCount == 1)
					return ShaderBuilder::Constant(value);
				else
					static_assert(AlwaysFalse<T>(), "unexpected TargetComponentCount");
			}
		};

		template<typename T, std::size_t TargetComponentCount>
		struct SwizzlePropagationBase<T, TargetComponentCount, 2>
		{
			std::unique_ptr<ConstantValueExpression> operator()(const std::array<UInt32, 4>& components, const Vector2<T>& value)
			{
				if constexpr (TargetComponentCount == 4)
					return ShaderBuilder::Constant(Vector4<T>(value[components[0]], value[components[1]], value[components[2]], value[components[3]]));
				else if constexpr (TargetComponentCount == 3)
					return ShaderBuilder::Constant(Vector3<T>(value[components[0]], value[components[1]], value[components[2]]));
				else if constexpr (TargetComponentCount == 2)
					return ShaderBuilder::Constant(Vector2<T>(value[components[0]], value[components[1]]));
				else if constexpr (TargetComponentCount == 1)
					return ShaderBuilder::Constant(value[components[0]]);
				else
					static_assert(AlwaysFalse<T>(), "unexpected TargetComponentCount");
			}
		};
		
		template<typename T, std::size_t TargetComponentCount>
		struct SwizzlePropagationBase<T, TargetComponentCount, 3>
		{
			std::unique_ptr<ConstantValueExpression> operator()(const std::array<UInt32, 4>& components, const Vector3<T>& value)
			{
				if constexpr (TargetComponentCount == 4)
					return ShaderBuilder::Constant(Vector4<T>(value[components[0]], value[components[1]], value[components[2]], value[components[3]]));
				else if constexpr (TargetComponentCount == 3)
					return ShaderBuilder::Constant(Vector3<T>(value[components[0]], value[components[1]], value[components[2]]));
				else if constexpr (TargetComponentCount == 2)
					return ShaderBuilder::Constant(Vector2<T>(value[components[0]], value[components[1]]));
				else if constexpr (TargetComponentCount == 1)
					return ShaderBuilder::Constant(value[components[0]]);
				else
					static_assert(AlwaysFalse<T>(), "unexpected TargetComponentCount");
			}
		};

		template<typename T, std::size_t TargetComponentCount>
		struct SwizzlePropagationBase<T, TargetComponentCount, 4>
		{
			std::unique_ptr<ConstantValueExpression> operator()(const std::array<UInt32, 4>& components, const Vector4<T>& value)
			{
				if constexpr (TargetComponentCount == 4)
					return ShaderBuilder::Constant(Vector4<T>(value[components[0]], value[components[1]], value[components[2]], value[components[3]]));
				else if constexpr (TargetComponentCount == 3)
					return ShaderBuilder::Constant(Vector3<T>(value[components[0]], value[components[1]], value[components[2]]));
				else if constexpr (TargetComponentCount == 2)
					return ShaderBuilder::Constant(Vector2<T>(value[components[0]], value[components[1]]));
				else if constexpr (TargetComponentCount == 1)
					return ShaderBuilder::Constant(value[components[0]]);
				else
					static_assert(AlwaysFalse<T>(), "unexpected TargetComponentCount");
			}
		};

		template<typename T, std::size_t... Args>
		struct SwizzlePropagation;

		/*************************************************************************************************/

		template<UnaryType Type, typename T>
		struct UnaryConstantPropagation;

		// LogicalNot
		template<typename T>
		struct UnaryLogicalNotBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T& arg)
			{
				return ShaderBuilder::Constant(!arg);
			}
		};

		template<typename T>
		struct UnaryLogicalNot;

		template<typename T>
		struct UnaryConstantPropagation<UnaryType::LogicalNot, T>
		{
			using Op = UnaryLogicalNot<T>;
		};

		// Minus
		template<typename T>
		struct UnaryMinusBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T& arg)
			{
				return ShaderBuilder::Constant(-arg);
			}
		};

		template<typename T>
		struct UnaryMinus;

		template<typename T>
		struct UnaryConstantPropagation<UnaryType::Minus, T>
		{
			using Op = UnaryMinus<T>;
		};

		// Plus
		template<typename T>
		struct UnaryPlusBase
		{
			std::unique_ptr<ConstantValueExpression> operator()(const T& arg)
			{
				return ShaderBuilder::Constant(arg);
			}
		};

		template<typename T>
		struct UnaryPlus;

		template<typename T>
		struct UnaryConstantPropagation<UnaryType::Plus, T>
		{
			using Op = UnaryPlus<T>;
		};

#define EnableOptimisation(Op, ...) template<> struct Op<__VA_ARGS__> : Op##Base<__VA_ARGS__> {}

		// Binary

		EnableOptimisation(BinaryCompEq, bool, bool);
		EnableOptimisation(BinaryCompEq, double, double);
		EnableOptimisation(BinaryCompEq, float, float);
		EnableOptimisation(BinaryCompEq, Int32, Int32);
		EnableOptimisation(BinaryCompEq, Vector2f, Vector2f);
		EnableOptimisation(BinaryCompEq, Vector3f, Vector3f);
		EnableOptimisation(BinaryCompEq, Vector4f, Vector4f);
		EnableOptimisation(BinaryCompEq, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryCompEq, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryCompEq, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryCompGe, double, double);
		EnableOptimisation(BinaryCompGe, float, float);
		EnableOptimisation(BinaryCompGe, Int32, Int32);
		EnableOptimisation(BinaryCompGe, Vector2f, Vector2f);
		EnableOptimisation(BinaryCompGe, Vector3f, Vector3f);
		EnableOptimisation(BinaryCompGe, Vector4f, Vector4f);
		EnableOptimisation(BinaryCompGe, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryCompGe, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryCompGe, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryCompGt, double, double);
		EnableOptimisation(BinaryCompGt, float, float);
		EnableOptimisation(BinaryCompGt, Int32, Int32);
		EnableOptimisation(BinaryCompGt, Vector2f, Vector2f);
		EnableOptimisation(BinaryCompGt, Vector3f, Vector3f);
		EnableOptimisation(BinaryCompGt, Vector4f, Vector4f);
		EnableOptimisation(BinaryCompGt, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryCompGt, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryCompGt, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryCompLe, double, double);
		EnableOptimisation(BinaryCompLe, float, float);
		EnableOptimisation(BinaryCompLe, Int32, Int32);
		EnableOptimisation(BinaryCompLe, Vector2f, Vector2f);
		EnableOptimisation(BinaryCompLe, Vector3f, Vector3f);
		EnableOptimisation(BinaryCompLe, Vector4f, Vector4f);
		EnableOptimisation(BinaryCompLe, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryCompLe, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryCompLe, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryCompLt, double, double);
		EnableOptimisation(BinaryCompLt, float, float);
		EnableOptimisation(BinaryCompLt, Int32, Int32);
		EnableOptimisation(BinaryCompLt, Vector2f, Vector2f);
		EnableOptimisation(BinaryCompLt, Vector3f, Vector3f);
		EnableOptimisation(BinaryCompLt, Vector4f, Vector4f);
		EnableOptimisation(BinaryCompLt, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryCompLt, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryCompLt, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryCompNe, bool, bool);
		EnableOptimisation(BinaryCompNe, double, double);
		EnableOptimisation(BinaryCompNe, float, float);
		EnableOptimisation(BinaryCompNe, Int32, Int32);
		EnableOptimisation(BinaryCompNe, Vector2f, Vector2f);
		EnableOptimisation(BinaryCompNe, Vector3f, Vector3f);
		EnableOptimisation(BinaryCompNe, Vector4f, Vector4f);
		EnableOptimisation(BinaryCompNe, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryCompNe, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryCompNe, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryLogicalAnd, bool, bool);
		EnableOptimisation(BinaryLogicalOr, bool, bool);

		EnableOptimisation(BinaryAddition, double, double);
		EnableOptimisation(BinaryAddition, float, float);
		EnableOptimisation(BinaryAddition, Int32, Int32);
		EnableOptimisation(BinaryAddition, Vector2f, Vector2f);
		EnableOptimisation(BinaryAddition, Vector3f, Vector3f);
		EnableOptimisation(BinaryAddition, Vector4f, Vector4f);
		EnableOptimisation(BinaryAddition, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryAddition, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryAddition, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryDivision, double, double);
		EnableOptimisation(BinaryDivision, double, Vector2d);
		EnableOptimisation(BinaryDivision, double, Vector3d);
		EnableOptimisation(BinaryDivision, double, Vector4d);
		EnableOptimisation(BinaryDivision, float, float);
		EnableOptimisation(BinaryDivision, float, Vector2f);
		EnableOptimisation(BinaryDivision, float, Vector3f);
		EnableOptimisation(BinaryDivision, float, Vector4f);
		EnableOptimisation(BinaryDivision, Int32, Int32);
		EnableOptimisation(BinaryDivision, Int32, Vector2i32);
		EnableOptimisation(BinaryDivision, Int32, Vector3i32);
		EnableOptimisation(BinaryDivision, Int32, Vector4i32);
		EnableOptimisation(BinaryDivision, Vector2f, float);
		EnableOptimisation(BinaryDivision, Vector2f, Vector2f);
		EnableOptimisation(BinaryDivision, Vector3f, float);
		EnableOptimisation(BinaryDivision, Vector3f, Vector3f);
		EnableOptimisation(BinaryDivision, Vector4f, float);
		EnableOptimisation(BinaryDivision, Vector4f, Vector4f);
		EnableOptimisation(BinaryDivision, Vector2d, double);
		EnableOptimisation(BinaryDivision, Vector2d, Vector2d);
		EnableOptimisation(BinaryDivision, Vector3d, double);
		EnableOptimisation(BinaryDivision, Vector3d, Vector3d);
		EnableOptimisation(BinaryDivision, Vector4d, double);
		EnableOptimisation(BinaryDivision, Vector4d, Vector4d);
		EnableOptimisation(BinaryDivision, Vector2i32, Int32);
		EnableOptimisation(BinaryDivision, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryDivision, Vector3i32, Int32);
		EnableOptimisation(BinaryDivision, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryDivision, Vector4i32, Int32);
		EnableOptimisation(BinaryDivision, Vector4i32, Vector4i32);

		EnableOptimisation(BinaryMultiplication, double, double);
		EnableOptimisation(BinaryMultiplication, double, Vector2d);
		EnableOptimisation(BinaryMultiplication, double, Vector3d);
		EnableOptimisation(BinaryMultiplication, double, Vector4d);
		EnableOptimisation(BinaryMultiplication, float, float);
		EnableOptimisation(BinaryMultiplication, float, Vector2f);
		EnableOptimisation(BinaryMultiplication, float, Vector3f);
		EnableOptimisation(BinaryMultiplication, float, Vector4f);
		EnableOptimisation(BinaryMultiplication, Int32, Int32);
		EnableOptimisation(BinaryMultiplication, Int32, Vector2i32);
		EnableOptimisation(BinaryMultiplication, Int32, Vector3i32);
		EnableOptimisation(BinaryMultiplication, Int32, Vector4i32);
		EnableOptimisation(BinaryMultiplication, Vector2f, float);
		EnableOptimisation(BinaryMultiplication, Vector2f, Vector2f);
		EnableOptimisation(BinaryMultiplication, Vector3f, float);
		EnableOptimisation(BinaryMultiplication, Vector3f, Vector3f);
		EnableOptimisation(BinaryMultiplication, Vector4f, float);
		EnableOptimisation(BinaryMultiplication, Vector4f, Vector4f);
		EnableOptimisation(BinaryMultiplication, Vector2d, double);
		EnableOptimisation(BinaryMultiplication, Vector2d, Vector2d);
		EnableOptimisation(BinaryMultiplication, Vector3d, double);
		EnableOptimisation(BinaryMultiplication, Vector3d, Vector3d);
		EnableOptimisation(BinaryMultiplication, Vector4d, double);
		EnableOptimisation(BinaryMultiplication, Vector4d, Vector4d);
		EnableOptimisation(BinaryMultiplication, Vector2i32, Int32);
		EnableOptimisation(BinaryMultiplication, Vector2i32, Vector2i32);
		EnableOptimisation(BinaryMultiplication, Vector3i32, Int32);
		EnableOptimisation(BinaryMultiplication, Vector3i32, Vector3i32);
		EnableOptimisation(BinaryMultiplication, Vector4i32, Int32);
		EnableOptimisation(BinaryMultiplication, Vector4i32, Vector4i32);

		EnableOptimisation(BinarySubtraction, double, double);
		EnableOptimisation(BinarySubtraction, float, float);
		EnableOptimisation(BinarySubtraction, Int32, Int32);
		EnableOptimisation(BinarySubtraction, Vector2f, Vector2f);
		EnableOptimisation(BinarySubtraction, Vector3f, Vector3f);
		EnableOptimisation(BinarySubtraction, Vector4f, Vector4f);
		EnableOptimisation(BinarySubtraction, Vector2i32, Vector2i32);
		EnableOptimisation(BinarySubtraction, Vector3i32, Vector3i32);
		EnableOptimisation(BinarySubtraction, Vector4i32, Vector4i32);

		// Cast

		EnableOptimisation(CastConstant, bool, bool);
		EnableOptimisation(CastConstant, bool, Int32);
		EnableOptimisation(CastConstant, bool, UInt32);

		EnableOptimisation(CastConstant, double, double);
		EnableOptimisation(CastConstant, double, float);
		EnableOptimisation(CastConstant, double, Int32);
		EnableOptimisation(CastConstant, double, UInt32);

		EnableOptimisation(CastConstant, float, double);
		EnableOptimisation(CastConstant, float, float);
		EnableOptimisation(CastConstant, float, Int32);
		EnableOptimisation(CastConstant, float, UInt32);

		EnableOptimisation(CastConstant, Int32, double);
		EnableOptimisation(CastConstant, Int32, float);
		EnableOptimisation(CastConstant, Int32, Int32);
		EnableOptimisation(CastConstant, Int32, UInt32);

		EnableOptimisation(CastConstant, UInt32, double);
		EnableOptimisation(CastConstant, UInt32, float);
		EnableOptimisation(CastConstant, UInt32, Int32);
		EnableOptimisation(CastConstant, UInt32, UInt32);

		//EnableOptimisation(CastConstant, Vector2d, double, double);
		//EnableOptimisation(CastConstant, Vector3d, double, double, double);
		//EnableOptimisation(CastConstant, Vector4d, double, double, double, double);

		EnableOptimisation(CastConstant, Vector2f, float, float);
		EnableOptimisation(CastConstant, Vector3f, float, float, float);
		EnableOptimisation(CastConstant, Vector4f, float, float, float, float);

		EnableOptimisation(CastConstant, Vector2i32, Int32, Int32);
		EnableOptimisation(CastConstant, Vector3i32, Int32, Int32, Int32);
		EnableOptimisation(CastConstant, Vector4i32, Int32, Int32, Int32, Int32);

		//EnableOptimisation(CastConstant, Vector2ui32, UInt32, UInt32);
		//EnableOptimisation(CastConstant, Vector3ui32, UInt32, UInt32, UInt32);
		//EnableOptimisation(CastConstant, Vector4ui32, UInt32, UInt32, UInt32, UInt32);

		// Swizzle
		EnableOptimisation(SwizzlePropagation, double, 1, 1);
		EnableOptimisation(SwizzlePropagation, double, 1, 2);
		EnableOptimisation(SwizzlePropagation, double, 1, 3);
		EnableOptimisation(SwizzlePropagation, double, 1, 4);

		EnableOptimisation(SwizzlePropagation, double, 2, 1);
		EnableOptimisation(SwizzlePropagation, double, 2, 2);
		EnableOptimisation(SwizzlePropagation, double, 2, 3);
		EnableOptimisation(SwizzlePropagation, double, 2, 4);

		EnableOptimisation(SwizzlePropagation, double, 3, 1);
		EnableOptimisation(SwizzlePropagation, double, 3, 2);
		EnableOptimisation(SwizzlePropagation, double, 3, 3);
		EnableOptimisation(SwizzlePropagation, double, 3, 4);

		EnableOptimisation(SwizzlePropagation, double, 4, 1);
		EnableOptimisation(SwizzlePropagation, double, 4, 2);
		EnableOptimisation(SwizzlePropagation, double, 4, 3);
		EnableOptimisation(SwizzlePropagation, double, 4, 4);

		EnableOptimisation(SwizzlePropagation, float, 1, 1);
		EnableOptimisation(SwizzlePropagation, float, 1, 2);
		EnableOptimisation(SwizzlePropagation, float, 1, 3);
		EnableOptimisation(SwizzlePropagation, float, 1, 4);

		EnableOptimisation(SwizzlePropagation, float, 2, 1);
		EnableOptimisation(SwizzlePropagation, float, 2, 2);
		EnableOptimisation(SwizzlePropagation, float, 2, 3);
		EnableOptimisation(SwizzlePropagation, float, 2, 4);

		EnableOptimisation(SwizzlePropagation, float, 3, 1);
		EnableOptimisation(SwizzlePropagation, float, 3, 2);
		EnableOptimisation(SwizzlePropagation, float, 3, 3);
		EnableOptimisation(SwizzlePropagation, float, 3, 4);

		EnableOptimisation(SwizzlePropagation, float, 4, 1);
		EnableOptimisation(SwizzlePropagation, float, 4, 2);
		EnableOptimisation(SwizzlePropagation, float, 4, 3);
		EnableOptimisation(SwizzlePropagation, float, 4, 4);

		EnableOptimisation(SwizzlePropagation, Int32, 1, 1);
		EnableOptimisation(SwizzlePropagation, Int32, 1, 2);
		EnableOptimisation(SwizzlePropagation, Int32, 1, 3);
		EnableOptimisation(SwizzlePropagation, Int32, 1, 4);

		EnableOptimisation(SwizzlePropagation, Int32, 2, 1);
		EnableOptimisation(SwizzlePropagation, Int32, 2, 2);
		EnableOptimisation(SwizzlePropagation, Int32, 2, 3);
		EnableOptimisation(SwizzlePropagation, Int32, 2, 4);

		EnableOptimisation(SwizzlePropagation, Int32, 3, 1);
		EnableOptimisation(SwizzlePropagation, Int32, 3, 2);
		EnableOptimisation(SwizzlePropagation, Int32, 3, 3);
		EnableOptimisation(SwizzlePropagation, Int32, 3, 4);

		EnableOptimisation(SwizzlePropagation, Int32, 4, 1);
		EnableOptimisation(SwizzlePropagation, Int32, 4, 2);
		EnableOptimisation(SwizzlePropagation, Int32, 4, 3);
		EnableOptimisation(SwizzlePropagation, Int32, 4, 4);

		// Unary

		EnableOptimisation(UnaryLogicalNot, bool);

		EnableOptimisation(UnaryMinus, double);
		EnableOptimisation(UnaryMinus, float);
		EnableOptimisation(UnaryMinus, Int32);
		EnableOptimisation(UnaryMinus, Vector2f);
		EnableOptimisation(UnaryMinus, Vector3f);
		EnableOptimisation(UnaryMinus, Vector4f);
		EnableOptimisation(UnaryMinus, Vector2i32);
		EnableOptimisation(UnaryMinus, Vector3i32);
		EnableOptimisation(UnaryMinus, Vector4i32);

		EnableOptimisation(UnaryPlus, double);
		EnableOptimisation(UnaryPlus, float);
		EnableOptimisation(UnaryPlus, Int32);
		EnableOptimisation(UnaryPlus, Vector2f);
		EnableOptimisation(UnaryPlus, Vector3f);
		EnableOptimisation(UnaryPlus, Vector4f);
		EnableOptimisation(UnaryPlus, Vector2i32);
		EnableOptimisation(UnaryPlus, Vector3i32);
		EnableOptimisation(UnaryPlus, Vector4i32);

#undef EnableOptimisation
	}

	ModulePtr AstConstantPropagationVisitor::Process(const Module& shaderModule)
	{
		auto rootnode = static_unique_pointer_cast<MultiStatement>(Process(*shaderModule.rootNode));

		return std::make_shared<Module>(shaderModule.metadata, std::move(rootnode), shaderModule.importedModules);
	}

	ModulePtr AstConstantPropagationVisitor::Process(const Module& shaderModule, const Options& options)
	{
		auto rootNode = static_unique_pointer_cast<MultiStatement>(Process(*shaderModule.rootNode, options));

		return std::make_shared<Module>(shaderModule.metadata, std::move(rootNode), shaderModule.importedModules);
	}

	ExpressionPtr AstConstantPropagationVisitor::Clone(BinaryExpression& node)
	{
		auto lhs = CloneExpression(node.left);
		auto rhs = CloneExpression(node.right);

		if (lhs->GetType() == NodeType::ConstantValueExpression && rhs->GetType() == NodeType::ConstantValueExpression)
		{
			const ConstantValueExpression& lhsConstant = static_cast<const ConstantValueExpression&>(*lhs);
			const ConstantValueExpression& rhsConstant = static_cast<const ConstantValueExpression&>(*rhs);

			ExpressionPtr optimized;
			switch (node.op)
			{
				case BinaryType::Add:
					optimized = PropagateBinaryConstant<BinaryType::Add>(lhsConstant, rhsConstant);
					break;

				case BinaryType::Subtract:
					optimized = PropagateBinaryConstant<BinaryType::Subtract>(lhsConstant, rhsConstant);
					break;

				case BinaryType::Multiply:
					optimized = PropagateBinaryConstant<BinaryType::Multiply>(lhsConstant, rhsConstant);
					break;

				case BinaryType::Divide:
					optimized = PropagateBinaryConstant<BinaryType::Divide>(lhsConstant, rhsConstant);
					break;

				case BinaryType::CompEq:
					optimized = PropagateBinaryConstant<BinaryType::CompEq>(lhsConstant, rhsConstant);
					break;

				case BinaryType::CompGe:
					optimized = PropagateBinaryConstant<BinaryType::CompGe>(lhsConstant, rhsConstant);
					break;

				case BinaryType::CompGt:
					optimized = PropagateBinaryConstant<BinaryType::CompGt>(lhsConstant, rhsConstant);
					break;

				case BinaryType::CompLe:
					optimized = PropagateBinaryConstant<BinaryType::CompLe>(lhsConstant, rhsConstant);
					break;

				case BinaryType::CompLt:
					optimized = PropagateBinaryConstant<BinaryType::CompLt>(lhsConstant, rhsConstant);
					break;

				case BinaryType::CompNe:
					optimized = PropagateBinaryConstant<BinaryType::CompNe>(lhsConstant, rhsConstant);
					break;

				case BinaryType::LogicalAnd:
					optimized = PropagateBinaryConstant<BinaryType::LogicalAnd>(lhsConstant, rhsConstant);
					break;

				case BinaryType::LogicalOr:
					optimized = PropagateBinaryConstant<BinaryType::LogicalOr>(lhsConstant, rhsConstant);
					break;
			}

			if (optimized)
				return optimized;
		}

		auto binary = ShaderBuilder::Binary(node.op, std::move(lhs), std::move(rhs));
		binary->cachedExpressionType = node.cachedExpressionType;

		return binary;
	}

	ExpressionPtr AstConstantPropagationVisitor::Clone(CastExpression& node)
	{
		std::array<ExpressionPtr, 4> expressions;

		std::size_t expressionCount = 0;
		for (const auto& expression : node.expressions)
		{
			if (!expression)
				break;

			expressions[expressionCount] = CloneExpression(expression);
			expressionCount++;
		}

		ExpressionPtr optimized;
		if (IsPrimitiveType(node.targetType.GetResultingValue()))
		{
			if (expressionCount == 1 && expressions.front()->GetType() == NodeType::ConstantValueExpression)
			{
				const ConstantValueExpression& constantExpr = static_cast<const ConstantValueExpression&>(*expressions.front());

				switch (std::get<PrimitiveType>(node.targetType.GetResultingValue()))
				{
					case PrimitiveType::Boolean: optimized = PropagateSingleValueCast<bool>(constantExpr); break;
					case PrimitiveType::Float32: optimized = PropagateSingleValueCast<float>(constantExpr); break;
					case PrimitiveType::Int32:   optimized = PropagateSingleValueCast<Int32>(constantExpr); break;
					case PrimitiveType::UInt32:  optimized = PropagateSingleValueCast<UInt32>(constantExpr); break;
					case PrimitiveType::String: break;
				}
			}
		}
		else if (IsVectorType(node.targetType.GetResultingValue()))
		{
			const auto& vecType = std::get<VectorType>(node.targetType.GetResultingValue());

			// Decompose vector into values (cast(vec3, float) => cast(float, float, float, float))
			std::vector<ConstantValue> constantValues;
			for (std::size_t i = 0; i < expressionCount; ++i)
			{
				if (expressions[i]->GetType() != NodeType::ConstantValueExpression)
				{
					constantValues.clear();
					break;
				}

				const auto& constantExpr = static_cast<ConstantValueExpression&>(*expressions[i]);

				if (!constantValues.empty() && GetExpressionType(constantValues.front()) != GetExpressionType(constantExpr.value))
				{
					// Unhandled case, all cast parameters are expected to be of the same type
					constantValues.clear();
					break;
				}

				std::visit([&](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::is_same_v<T, NoValue>)
						throw std::runtime_error("invalid type (value expected)");
					else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, Int32> || std::is_same_v<T, UInt32> || std::is_same_v<T, std::string>)
						constantValues.push_back(arg);
					else if constexpr (std::is_same_v<T, Vector2f> || std::is_same_v<T, Vector2i32>)
					{
						constantValues.push_back(arg.x);
						constantValues.push_back(arg.y);
					}
					else if constexpr (std::is_same_v<T, Vector3f> || std::is_same_v<T, Vector3i32>)
					{
						constantValues.push_back(arg.x);
						constantValues.push_back(arg.y);
						constantValues.push_back(arg.z);
					}
					else if constexpr (std::is_same_v<T, Vector4f> || std::is_same_v<T, Vector4i32>)
					{
						constantValues.push_back(arg.x);
						constantValues.push_back(arg.y);
						constantValues.push_back(arg.z);
						constantValues.push_back(arg.w);
					}
					else
						static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
				}, constantExpr.value);
			}

			if (!constantValues.empty())
			{
				assert(constantValues.size() == vecType.componentCount);

				std::visit([&](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;

					switch (vecType.componentCount)
					{
						case 2:
							optimized = PropagateVec2Cast(std::get<T>(constantValues[0]), std::get<T>(constantValues[1]));
							break;

						case 3:
							optimized = PropagateVec3Cast(std::get<T>(constantValues[0]), std::get<T>(constantValues[1]), std::get<T>(constantValues[2]));
							break;

						case 4:
							optimized = PropagateVec4Cast(std::get<T>(constantValues[0]), std::get<T>(constantValues[1]), std::get<T>(constantValues[2]), std::get<T>(constantValues[3]));
							break;
					}
				}, constantValues.front());
			}
		}

		if (optimized)
			return optimized;
		
		auto cast = ShaderBuilder::Cast(node.targetType.GetResultingValue(), std::move(expressions));
		cast->cachedExpressionType = node.cachedExpressionType;

		return cast;
	}

	StatementPtr AstConstantPropagationVisitor::Clone(BranchStatement& node)
	{
		std::vector<BranchStatement::ConditionalStatement> statements;
		StatementPtr elseStatement;

		for (auto& condStatement : node.condStatements)
		{
			auto cond = CloneExpression(condStatement.condition);

			if (cond->GetType() == NodeType::ConstantValueExpression)
			{
				auto& constant = static_cast<ConstantValueExpression&>(*cond);

				const ExpressionType& constantType = GetExpressionType(constant);
				if (!IsPrimitiveType(constantType) || std::get<PrimitiveType>(constantType) != PrimitiveType::Boolean)
					continue;

				bool cValue = std::get<bool>(constant.value);
				if (!cValue)
					continue;

				if (statements.empty())
				{
					// First condition is true, dismiss the branch
					return Unscope(AstCloner::Clone(*condStatement.statement));
				}
				else
				{
					// Some condition after the first one is true, make it the else statement and stop there
					elseStatement = CloneStatement(condStatement.statement);
					break;
				}
			}
			else
			{
				auto& c = statements.emplace_back();
				c.condition = std::move(cond);
				c.statement = CloneStatement(condStatement.statement);
			}
		}

		if (statements.empty())
		{
			// All conditions have been removed, replace by else statement or no-op
			if (node.elseStatement)
				return Unscope(AstCloner::Clone(*node.elseStatement));
			else
				return ShaderBuilder::NoOp();
		}

		if (!elseStatement)
			elseStatement = CloneStatement(node.elseStatement);

		return ShaderBuilder::Branch(std::move(statements), std::move(elseStatement));
	}

	ExpressionPtr AstConstantPropagationVisitor::Clone(ConditionalExpression& node)
	{
		auto cond = CloneExpression(node.condition);
		if (cond->GetType() != NodeType::ConstantValueExpression)
			throw std::runtime_error("conditional expression condition must be a constant expression");

		auto& constant = static_cast<ConstantValueExpression&>(*cond);

		assert(constant.cachedExpressionType);
		const ExpressionType& constantType = constant.cachedExpressionType.value();

		if (!IsPrimitiveType(constantType) || std::get<PrimitiveType>(constantType) != PrimitiveType::Boolean)
			throw std::runtime_error("conditional expression condition must resolve to a boolean");

		bool cValue = std::get<bool>(constant.value);
		if (cValue)
			return AstCloner::Clone(*node.truePath);
		else
			return AstCloner::Clone(*node.falsePath);
	}

	ExpressionPtr AstConstantPropagationVisitor::Clone(ConstantExpression& node)
	{
		if (!m_options.constantQueryCallback)
			return AstCloner::Clone(node);

		auto constant = ShaderBuilder::Constant(m_options.constantQueryCallback(node.constantId));
		constant->cachedExpressionType = GetExpressionType(constant->value);

		return constant;
	}

	ExpressionPtr AstConstantPropagationVisitor::Clone(SwizzleExpression& node)
	{
		auto expr = CloneExpression(node.expression);

		if (expr->GetType() == NodeType::ConstantValueExpression)
		{
			const ConstantValueExpression& constantExpr = static_cast<const ConstantValueExpression&>(*expr);

			ExpressionPtr optimized;
			switch (node.componentCount)
			{
				case 1:
					optimized = PropagateConstantSwizzle<1>(node.components, constantExpr);
					break;

				case 2:
					optimized = PropagateConstantSwizzle<2>(node.components, constantExpr);
					break;

				case 3:
					optimized = PropagateConstantSwizzle<3>(node.components, constantExpr);
					break;

				case 4:
					optimized = PropagateConstantSwizzle<4>(node.components, constantExpr);
					break;
			}

			if (optimized)
				return optimized;
		}
		else if (expr->GetType() == NodeType::SwizzleExpression)
		{
			SwizzleExpression& constantExpr = static_cast<SwizzleExpression&>(*expr);

			std::array<UInt32, 4> newComponents = {};
			for (std::size_t i = 0; i < node.componentCount; ++i)
				newComponents[i] = constantExpr.components[node.components[i]];
			
			constantExpr.componentCount = node.componentCount;
			constantExpr.components = newComponents;

			return expr;
		}

		auto swizzle = ShaderBuilder::Swizzle(std::move(expr), node.components, node.componentCount);
		swizzle->cachedExpressionType = node.cachedExpressionType;

		return swizzle;
	}

	ExpressionPtr AstConstantPropagationVisitor::Clone(UnaryExpression& node)
	{
		auto expr = CloneExpression(node.expression);

		if (expr->GetType() == NodeType::ConstantValueExpression)
		{
			const ConstantValueExpression& constantExpr = static_cast<const ConstantValueExpression&>(*expr);

			ExpressionPtr optimized;
			switch (node.op)
			{
				case UnaryType::LogicalNot:
					optimized = PropagateUnaryConstant<UnaryType::LogicalNot>(constantExpr);
					break;

				case UnaryType::Minus:
					optimized = PropagateUnaryConstant<UnaryType::Minus>(constantExpr);
					break;

				case UnaryType::Plus:
					optimized = PropagateUnaryConstant<UnaryType::Plus>(constantExpr);
					break;
			}

			if (optimized)
				return optimized;
		}

		auto unary = ShaderBuilder::Unary(node.op, std::move(expr));
		unary->cachedExpressionType = node.cachedExpressionType;

		return unary;
	}

	StatementPtr AstConstantPropagationVisitor::Clone(ConditionalStatement& node)
	{
		auto cond = CloneExpression(node.condition);
		if (cond->GetType() != NodeType::ConstantValueExpression)
			throw std::runtime_error("conditional expression condition must be a constant expression");

		auto& constant = static_cast<ConstantValueExpression&>(*cond);

		assert(constant.cachedExpressionType);
		const ExpressionType& constantType = constant.cachedExpressionType.value();

		if (!IsPrimitiveType(constantType) || std::get<PrimitiveType>(constantType) != PrimitiveType::Boolean)
			throw std::runtime_error("conditional expression condition must resolve to a boolean");

		bool cValue = std::get<bool>(constant.value);
		if (cValue)
			return AstCloner::Clone(node);
		else
			return ShaderBuilder::NoOp();
	}

	template<BinaryType Type>
	ExpressionPtr AstConstantPropagationVisitor::PropagateBinaryConstant(const ConstantValueExpression& lhs, const ConstantValueExpression& rhs)
	{
		std::unique_ptr<ConstantValueExpression> optimized;
		std::visit([&](auto&& arg1)
		{
			using T1 = std::decay_t<decltype(arg1)>;

			std::visit([&](auto&& arg2)
			{
				using T2 = std::decay_t<decltype(arg2)>;
				using PCType = BinaryConstantPropagation<Type, T1, T2>;

				if constexpr (is_complete_v<PCType>)
				{
					using Op = typename PCType::Op;
					if constexpr (is_complete_v<Op>)
						optimized = Op{}(arg1, arg2);
				}

			}, rhs.value);
		}, lhs.value);

		if (optimized)
			optimized->cachedExpressionType = GetExpressionType(optimized->value);

		return optimized;
	}

	template<typename TargetType>
	ExpressionPtr AstConstantPropagationVisitor::PropagateSingleValueCast(const ConstantValueExpression& operand)
	{
		std::unique_ptr<ConstantValueExpression> optimized;

		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			using CCType = CastConstant<TargetType, T>;

			if constexpr (is_complete_v<CCType>)
				optimized = CCType{}(arg);
		}, operand.value);

		return optimized;
	}

	template<std::size_t TargetComponentCount>
	ExpressionPtr AstConstantPropagationVisitor::PropagateConstantSwizzle(const std::array<UInt32, 4>& components, const ConstantValueExpression& operand)
	{
		std::unique_ptr<ConstantValueExpression> optimized;
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			using BaseType = typename VectorInfo<T>::Base;
			constexpr std::size_t FromComponentCount = VectorInfo<T>::Dimensions;

			using SPType = SwizzlePropagation<BaseType, TargetComponentCount, FromComponentCount>;

			if constexpr (is_complete_v<SPType>)
				optimized = SPType{}(components, arg);
		}, operand.value);

		return optimized;
	}

	template<UnaryType Type>
	ExpressionPtr AstConstantPropagationVisitor::PropagateUnaryConstant(const ConstantValueExpression& operand)
	{
		std::unique_ptr<ConstantValueExpression> optimized;
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			using PCType = UnaryConstantPropagation<Type, T>;

			if constexpr (is_complete_v<PCType>)
			{
				using Op = typename PCType::Op;
				if constexpr (is_complete_v<Op>)
					optimized = Op{}(arg);
			}
		}, operand.value);

		if (optimized)
			optimized->cachedExpressionType = GetExpressionType(optimized->value);

		return optimized;
	}

	template<typename TargetType>
	ExpressionPtr AstConstantPropagationVisitor::PropagateVec2Cast(TargetType v1, TargetType v2)
	{
		std::unique_ptr<ConstantValueExpression> optimized;

		using CCType = CastConstant<Vector2<TargetType>, TargetType, TargetType>;

		if constexpr (is_complete_v<CCType>)
			optimized = CCType{}(v1, v2);

		return optimized;
	}

	template<typename TargetType>
	ExpressionPtr AstConstantPropagationVisitor::PropagateVec3Cast(TargetType v1, TargetType v2, TargetType v3)
	{
		std::unique_ptr<ConstantValueExpression> optimized;

		using CCType = CastConstant<Vector3<TargetType>, TargetType, TargetType, TargetType>;

		if constexpr (is_complete_v<CCType>)
			optimized = CCType{}(v1, v2, v3);

		return optimized;
	}

	template<typename TargetType>
	ExpressionPtr AstConstantPropagationVisitor::PropagateVec4Cast(TargetType v1, TargetType v2, TargetType v3, TargetType v4)
	{
		std::unique_ptr<ConstantValueExpression> optimized;

		using CCType = CastConstant<Vector4<TargetType>, TargetType, TargetType, TargetType, TargetType>;

		if constexpr (is_complete_v<CCType>)
			optimized = CCType{}(v1, v2, v3, v4);

		return optimized;
	}


	StatementPtr AstConstantPropagationVisitor::Unscope(StatementPtr node)
	{
		assert(node);

		if (node->GetType() == NodeType::ScopedStatement)
			return std::move(static_cast<ScopedStatement&>(*node).statement);
		else
			return node;
	}
}
