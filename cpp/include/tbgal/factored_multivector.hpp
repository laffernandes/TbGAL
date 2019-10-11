#ifndef __TBGAL_MULTIVECTOR_HPP__
#define __TBGAL_MULTIVECTOR_HPP__

namespace tbgal {

    template<typename FactoringProductType, typename SquareMatrixType>
    class FactoredMultivector final {
    public:

        static_assert(is_factoring_product_v<FactoringProductType>, "Invalid FactoringProductType.");

        using IndexType = detail::index_type_t<SquareMatrixType>;
        using ScalarType = detail::scalar_type_t<SquareMatrixType>;

        using MetricSpaceType = typename FactoringProductType::MetricSpaceType;

        constexpr FactoredMultivector(FactoredMultivector const &) = default;
        constexpr FactoredMultivector(FactoredMultivector &&) = default;

        constexpr FactoredMultivector(MetricSpaceType const &space) noexcept :
            space_(space),
            scalar_(0),
            factors_in_signed_metric_(detail::make_identity_matrix<ScalarType, MetricSpaceType::DimensionsAtCompileTime>(space.dimensions())),
            factors_count_(0) {
        }

        template<typename OtherScalarType>
        constexpr FactoredMultivector(MetricSpaceType const &space, OtherScalarType &&scalar) noexcept :
            space_(space),
            scalar_(std::move(scalar)),
            factors_in_signed_metric_(detail::make_identity_matrix<ScalarType, MetricSpaceType::DimensionsAtCompileTime>(space.dimensions())),
            factors_count_(0) {
        }

        //TODO [FUTURE] Specialization.
        //template<typename OtherFactoringProductType, typename OtherSquareMatrixType>
        //constexpr FactoredMultivector(FactoredMultivector<OtherFactoringProductType, OtherSquareMatrixType> const &other) noexcept;

        constexpr FactoredMultivector & operator=(FactoredMultivector const &) = default;
        constexpr FactoredMultivector & operator=(FactoredMultivector &&) = default;

        //TODO [FUTURE] Specialization.
        //template<typename OtherFactoringProductType, typename OtherSquareMatrixType>
        //constexpr FactoredMultivector & operator=(FactoredMultivector<OtherFactoringProductType, OtherSquareMatrixType> const &) = default;

        constexpr MetricSpaceType const & space() const noexcept {
            return space_;
        }

        constexpr ScalarType const & scalar() const noexcept {
            return scalar_;
        }

        constexpr decltype(auto) factors_in_actual_metric() const noexcept {
            return detail::from_signed_to_actual_metric(space_, factors_in_signed_metric_);
        }

        constexpr SquareMatrixType const & factors_in_signed_metric() const noexcept {
            return factors_in_signed_metric_;
        }

        constexpr IndexType const & factors_count() const noexcept {
            return factors_count_;
        }

    private:

        template<typename OtherScalarType, typename OtherSquareMatrixType, typename OtherIndexType>
        constexpr FactoredMultivector(MetricSpaceType const &space, OtherScalarType &&scalar, OtherSquareMatrixType &&factors_in_signed_metric, OtherIndexType &&factors_count) noexcept :
            space_(space),
            scalar_(std::move(scalar)),
            factors_in_signed_metric_(std::move(factors_in_signed_metric)),
            factors_count_(std::move(factors_count)) {
        }

    private:

        MetricSpaceType const &space_;

        ScalarType scalar_;

        SquareMatrixType factors_in_signed_metric_;
        IndexType factors_count_;

        template<bool AnyMultivectorType> friend struct detail::OP_impl;

        template<typename SomeScalarType, typename SomeMetricSpaceType> friend decltype(auto) e(SomeMetricSpaceType const &, DefaultIndexType) noexcept;
        template<typename SomeMetricSpaceType, typename SomeScalarType, typename> friend decltype(auto) scalar(SomeMetricSpaceType const &, SomeScalarType const &) noexcept;
        template<typename SomeMetricSpaceType, typename... ScalarTypes> friend decltype(auto) vector(SomeMetricSpaceType const &, ScalarTypes &&...) noexcept;

        template<typename FirstFactoringProductType, typename FirstSquareMatrixType, typename SecondFactoringProductType, typename SecondSquareMatrixType> friend constexpr decltype(auto) ADD(FactoredMultivector<FirstFactoringProductType, FirstSquareMatrixType> const &, FactoredMultivector<SecondFactoringProductType, SecondSquareMatrixType> const &) noexcept;
        template<typename SomeMetricSpaceType, typename SomeSquareMatrixType> friend constexpr decltype(auto) DUAL(FactoredMultivector<OuterProduct<SomeMetricSpaceType>, SomeSquareMatrixType> const &) noexcept;
        template<typename FirstScalarType, typename SecondFactoringProduct, typename SecondSquareMatrixType, typename> friend constexpr decltype(auto) LCONT(FirstScalarType const &, FactoredMultivector<SecondFactoringProduct, SecondSquareMatrixType> const &) noexcept;
        template<typename SomeFactoringProductType, typename SomeSquareMatrixType> friend constexpr decltype(auto) REVERSE(FactoredMultivector<SomeFactoringProductType, SomeSquareMatrixType> const &) noexcept;
        template<typename FirstFactoringProductType, typename FirstSquareMatrixType, typename SecondFactoringProductType, typename SecondSquareMatrixType> friend constexpr decltype(auto) SUB(FactoredMultivector<FirstFactoringProductType, FirstSquareMatrixType> const &, FactoredMultivector<SecondFactoringProductType, SecondSquareMatrixType> const &) noexcept;
        template<typename SomeFactoringProductType, typename SomeSquareMatrixType> friend constexpr FactoredMultivector<SomeFactoringProductType, SomeSquareMatrixType> UNARY_MINUS(FactoredMultivector<SomeFactoringProductType, SomeSquareMatrixType> const &) noexcept;
        template<typename SomeMetricSpaceType, typename SomeSquareMatrixType> friend constexpr decltype(auto) UNDUAL(FactoredMultivector<OuterProduct<SomeMetricSpaceType>, SomeSquareMatrixType> const &) noexcept;
    };

    template<typename FactoringProductType, typename SquareMatrixType>
    struct is_multivector<FactoredMultivector<FactoringProductType, SquareMatrixType> > :
        std::true_type {
    };

}

#endif // __TBGAL_MULTIVECTOR_HPP__
