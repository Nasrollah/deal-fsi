
#pragma once

/*
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#include <Eigen/Dense>
#include <map>
#include <fstream>
#include <iostream>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/base/logstream.h>
#include <deal.II/dofs/dof_renumbering.h>
#include <deal.II/lac/sparse_direct.h>
#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/constraint_matrix.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/base/utilities.h>
#include <deal.II/base/conditional_ostream.h>
#include <deal.II/lac/petsc_parallel_vector.h>
#include <deal.II/lac/petsc_parallel_sparse_matrix.h>
#include <deal.II/lac/petsc_solver.h>
#include <deal.II/lac/petsc_precondition.h>

#include "RightHandSide.h"
#include "DataStorage.h"

namespace dealiifsi
{
    using namespace dealii;

    template <class Scalar>
    Vector<Scalar> operator *(
        const SparseMatrix<Scalar> & A,
        const Vector<Scalar> & b
        );

    template <class Scalar>
    Vector<Scalar> operator *(
        const Scalar & scalar,
        const Vector<Scalar> & vector
        );

    template <class Scalar>
    Vector<Scalar> operator -(
        const Vector<Scalar> & A,
        const Vector<Scalar> & B
        );

    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrix;

    template <int dim>
    class LinearElasticity
    {
public:

        explicit LinearElasticity( DataStorage & data );

        LinearElasticity(
            double time_step,
            double final_time,
            double theta,
            double degree,
            double gravity,
            double distributed_load,
            double rho,
            double E,
            double nu,
            unsigned int n_global_refines
            );

        void assemble_system();

        void finalizeTimeStep();

        double get_traction(
            const unsigned int component_i,
            const unsigned int
            );

        void getDisplacement( EigenMatrix & displacement );

        void getReadPositions( EigenMatrix & readPositions );

        void getWritePositions( EigenMatrix & writePositions );

        void setTraction( const EigenMatrix & traction );

        void initialize();

        void initTimeStep();

        bool isRunning();

        void output_results() const;

        unsigned int n_dofs() const;

        double point_value() const;

        void run();

        void setup_system();

        void solve();

        void solve_u();

        const unsigned int deg;
        const unsigned int n_global_refines;

        Triangulation<dim>   triangulation;
        FESystem<dim>        fe;
        DoFHandler<dim>      dof_handler;

        ConstraintMatrix constraints;

        PETScWrappers::MPI::SparseMatrix mass_matrix, laplace_matrix, matrix_u;
        PETScWrappers::MPI::Vector solution_u, solution_v, old_solution_u, old_solution_v;
        PETScWrappers::MPI::Vector system_rhs, body_force, old_body_force;

        double initial_time, final_time, time, time_step;
        unsigned int timestep_number;
        const double theta;
        const double gravity, distributed_load;

protected:

        ConditionalOStream pcout;

        bool init;
        const double rho, E, nu;
        const bool output_paraview;
        const int output_interval;

        std::map<unsigned int, unsigned int> dof_index_to_boundary_index;
        EigenMatrix traction;

        // SDC time integration variables
        PETScWrappers::MPI::Vector u_f, v_f, u_rhs, v_rhs;

        MPI_Comm mpi_communicator;
        const unsigned int n_mpi_processes, this_mpi_process;
    };

    #include "../src/LinearElasticity.tpp"
}
