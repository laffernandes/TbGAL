#ifndef __TBGAL_METRIC_SPACE_HPP__
#define __TBGAL_METRIC_SPACE_HPP__

namespace tbgal {

    template<typename SymmetricMatrixType>
    class MetricSpace {
    public:

        using IndexType = detail::index_type_t<SymmetricMatrixType>;
        
        constexpr auto DimensionsAtCompileTime = detail::rows_at_compile_time_v<SymmetricMatrixType>;

        constexpr MetricSpace(MetricSpace const &) = default;
        constexpr MetricSpace(MetricSpace &&) = default;

        constexpr IndexType dimensions() const noexcept {
            return detail::rows(metric_matrix_);
        }

    protected:

        constexpr MetricSpace(SymmetricMatrixType &&metric_matrix) noexcept :
            metric_matrix_{ std::move(metric_matrix) } {
        }

        constexpr SymmetricMatrixType metric_matrix_;
    };

    namespace detail {

        template<typename SymmetricMatrixType>
        struct is_metric_space<MetricSpace<SymmetricMatrixType> > :
            std::true_type {
        };

    }

}

#endif // __TBGAL_METRIC_SPACE_HPP__
