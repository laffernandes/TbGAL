/* Copyright (C) Eduardo Vera Sousa and Leandro Augusto Frata Fernandes
 * 
 * authors    : Sousa, Eduardo V.
 *              Fernandes, Leandro A. F.
 * repository : https://github.com/Prograf-UFF/TbGAL
 * 
 * This file is part of the Tensor-based Geometric Algebra Library (TbGAL).
 * 
 * TbGAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TbGAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TbGAL. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __TBGAL_UTILS_HPP__
#define __TBGAL_UTILS_HPP__

#ifndef TBGAL_DEFAULT_FLT_TOLERANCE
    #define TBGAL_DEFAULT_FLT_TOLERANCE 1.0e-8f
#else
    static_assert(TBGAL_DEFAULT_FLT_TOLERANCE >= 0, "TBGAL_DEFAULT_FLT_TOLERANCE must be a non-negative value.")
#endif // TBGAL_DEFAULT_FLT_TOLERANCE

#ifndef TBGAL_DEFAULT_DBL_TOLERANCE
    #define TBGAL_DEFAULT_DBL_TOLERANCE 1.0e-8
#else
    static_assert(TBGAL_DEFAULT_DBL_TOLERANCE >= 0, "TBGAL_DEFAULT_DBL_TOLERANCE must be a non-negative value.")
#endif // TBGAL_DEFAULT_DBL_TOLERANCE

namespace tbgal {

    template<typename ValueType>
    constexpr decltype(auto) default_tolerance() noexcept;

    template<>
    constexpr decltype(auto) default_tolerance<std::float_t>() noexcept {
        return TBGAL_DEFAULT_FLT_TOLERANCE;
    }

    template<>
    constexpr decltype(auto) default_tolerance<std::double_t>() noexcept {
        return TBGAL_DEFAULT_DBL_TOLERANCE;
    }

    template<>
    constexpr decltype(auto) default_tolerance<std::int8_t>() noexcept {
        return 0;
    }

    template<>
    constexpr decltype(auto) default_tolerance<std::int16_t>() noexcept {
        return 0;
    }

    template<>
    constexpr decltype(auto) default_tolerance<std::int32_t>() noexcept {
        return 0;
    }

    template<>
    constexpr decltype(auto) default_tolerance<std::int64_t>() noexcept {
        return 0;
    }

    template<typename Type>
    constexpr bool is_blade(Type const &arg) noexcept {
        return true;
    }

    template<typename ScalarType, typename MetricSpaceType>
    constexpr bool is_blade(FactoredMultivector<ScalarType, GeometricProduct<MetricSpaceType> > const &arg) noexcept {
        return arg.factors_count() == 0 || arg.factors_count() == 1 || is_zero(arg.scalar());
        //TODO [FUTURE] Implement the general case.
    }

    template<typename ScalarType, typename MetricSpaceType>
    constexpr bool is_blade(FactoredMultivector<ScalarType, OuterProduct<MetricSpaceType> > const &arg) noexcept {
        return true;
    }

    template<typename ScalarType>
    constexpr bool is_zero(ScalarType const &arg) noexcept {
        return abs(arg) <= default_tolerance<ScalarType>();
    }

    template<typename ScalarType, typename FactoringProductType>
    constexpr bool is_zero(FactoredMultivector<ScalarType, FactoringProductType> const &arg) noexcept {
        return arg.factors_count() == 0 && abs(arg.scalar()) <= default_tolerance<ScalarType>();
    }

    template<typename MetricSpaceType, typename ScalarType, typename = std::enable_if_t<!is_multivector_v<std::remove_cv_t<std::remove_reference_t<ScalarType> > > > >
    decltype(auto) scalar(MetricSpaceType const *space_ptr, ScalarType &&value) noexcept {
        using ResultingScalarType = std::remove_cv_t<std::remove_reference_t<ScalarType> >;
        using ResultingFactoringProductType = OuterProduct<MetricSpaceType>;
        using ResultingFactoredMultivectorType = FactoredMultivector<ResultingScalarType, ResultingFactoringProductType>;
        return ResultingFactoredMultivectorType(space_ptr, std::move(value));
    }

    namespace detail {

        template<typename MetricSpaceType, typename... ScalarTypes>
        constexpr decltype(auto) make_vector(MetricSpaceType const *space_ptr, ScalarTypes &&... coords) noexcept {
            using ResultingScalarType = std::common_type_t<std::remove_cv_t<std::remove_reference_t<ScalarTypes> >...>;
            using ResultingFactoringProductType = OuterProduct<MetricSpaceType>;
            using ResultingFactoredMultivectorType = FactoredMultivector<ResultingScalarType, ResultingFactoringProductType>;
            static_assert(MetricSpaceType::DimensionsAtCompileTime == Dynamic || MetricSpaceType::DimensionsAtCompileTime == sizeof...(ScalarTypes), "Invalid number of coordinates.");
            assert(space_ptr->dimensions() == sizeof...(ScalarTypes));
            auto input = detail::evaluate(detail::from_actual_to_signed_metric(space_ptr, detail::fill_column_matrix(std::move(coords)...)));
            auto qr_tuple = detail::qr_orthogonal_matrix(input);
            if (std::get<1>(qr_tuple) == 1) {
                auto const &matrix_q = std::get<0>(qr_tuple);
                return ResultingFactoredMultivectorType(
                    space_ptr,
                    detail::coeff(detail::prod_block<1, MetricSpaceType::DimensionsAtCompileTime>(detail::transpose(matrix_q), 0, 0, 1, space_ptr->dimensions(), input), 0, 0),
                    matrix_q,
                    1
                );
            }
            return ResultingFactoredMultivectorType(space_ptr, 0);
        };

        template<typename MetricSpaceType, typename IteratorType, typename... ExtraScalarTypes>
        constexpr decltype(auto) make_vector_using_iterator(MetricSpaceType const *space_ptr, IteratorType begin, IteratorType end, ExtraScalarTypes &&... extra_coords) noexcept {
            using ResultingScalarType = std::common_type_t<std::remove_cv_t<std::remove_reference_t<typename std::iterator_traits<IteratorType>::value_type> >, std::remove_cv_t<std::remove_reference_t<ExtraScalarTypes> >...>;
            using ResultingFactoringProductType = OuterProduct<MetricSpaceType>;
            using ResultingFactoredMultivectorType = FactoredMultivector<ResultingScalarType, ResultingFactoringProductType>;
            assert(space_ptr->dimensions() == (std::distance(begin, end) + sizeof...(extra_coords)));
            auto input = detail::evaluate(detail::from_actual_to_signed_metric(space_ptr, detail::fill_column_matrix_using_iterator<MetricSpaceType::DimensionsAtCompileTime, MetricSpaceType::MaxDimensionsAtCompileTime>(begin, end, std::move(extra_coords)...)));
            auto qr_tuple = detail::qr_orthogonal_matrix(input);

            if (std::get<1>(qr_tuple) == 1) {
                auto const &matrix_q = std::get<0>(qr_tuple);
                return ResultingFactoredMultivectorType(
                    space_ptr,
                    detail::coeff(detail::prod_block<1, MetricSpaceType::DimensionsAtCompileTime>(detail::transpose(matrix_q), 0, 0, 1, space_ptr->dimensions(), input), 0, 0),
                    matrix_q,
                    1
                );
            }
            return ResultingFactoredMultivectorType(space_ptr, 0);
        };

    }
    
    template<typename MetricSpaceType, typename... ScalarTypes, typename = std::enable_if_t<std::disjunction_v<std::bool_constant<!detail::is_iterator_v<ScalarTypes> >...> > >
    decltype(auto) vector(MetricSpaceType const *space_ptr, ScalarTypes &&... coords) noexcept {
        return detail::make_vector(space_ptr, std::move(coords)...);
    }

    template<typename MetricSpaceType, typename IteratorType, typename = std::enable_if_t<detail::is_iterator_v<IteratorType> > >
    decltype(auto) vector(MetricSpaceType const *space_ptr, IteratorType begin, IteratorType end) noexcept {
        return detail::make_vector_using_iterator(space_ptr, begin, end);
    }

    template<typename ScalarType, typename MetricSpaceType>
    decltype(auto) e(MetricSpaceType const *space_ptr, DefaultIndexType index) noexcept {
        using ResultingScalarType = ScalarType;
        using ResultingFactoringProductType = OuterProduct<MetricSpaceType>;
        using ResultingFactoredMultivectorType = FactoredMultivector<ResultingScalarType, ResultingFactoringProductType>;
        assert(1 <= index && index <= space_ptr->dimensions());
        auto aux = detail::make_matrix<ScalarType, MetricSpaceType::DimensionsAtCompileTime, 1, MetricSpaceType::MaxDimensionsAtCompileTime, 1>(space_ptr->dimensions(), 1);
        detail::assign_block<Dynamic, 1>(detail::make_zero_matrix<ScalarType, Dynamic, 1, MetricSpaceType::DimensionsAtCompileTime, 1>(index - 1, 1), aux, 0, 0, index - 1, 1);
        detail::coeff(aux, index - 1, 0) = 1;
        detail::assign_block<Dynamic, 1>(detail::make_zero_matrix<ScalarType, Dynamic, 1, MetricSpaceType::DimensionsAtCompileTime, 1>(space_ptr->dimensions() - index, 1), aux, index, 0, space_ptr->dimensions() - index, 1);
        auto input = detail::evaluate(detail::from_actual_to_signed_metric(space_ptr, aux));
        auto qr_tuple = detail::qr_orthogonal_matrix(input);
        auto const &matrix_q = std::get<0>(qr_tuple);
        return ResultingFactoredMultivectorType(
            space_ptr,
            detail::coeff(detail::prod_block<1, MetricSpaceType::DimensionsAtCompileTime>(detail::transpose(matrix_q), 0, 0, 1, space_ptr->dimensions(), input), 0, 0),
            matrix_q,
            1
        );
    }

}

#endif // __TBGAL_UTILS_HPP__