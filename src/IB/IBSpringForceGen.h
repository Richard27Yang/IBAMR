#ifndef included_IBSpringForceGen
#define included_IBSpringForceGen

// Filename: IBSpringForceGen.h
// Last modified: <21.Mar.2007 22:26:51 griffith@box221.cims.nyu.edu>
// Created on 14 Jul 2004 by Boyce Griffith (boyce@trasnaform.speakeasy.net)

/////////////////////////////// INCLUDES /////////////////////////////////////

// IBAMR INCLUDES
#include <ibamr/IBLagrangianForceStrategy.h>
#include <ibamr/LDataManager.h>
#include <ibamr/LNodeLevelData.h>

// SAMRAI INCLUDES
#include <PatchHierarchy.h>
#include <tbox/Database.h>
#include <tbox/Pointer.h>

// PETSc INCLUDES
#include <petscmat.h>

// C++ STDLIB INCLUDES
#include <vector>

/////////////////////////////// CLASS DEFINITION /////////////////////////////

namespace IBAMR
{

/*!
 * \brief Function to compute the force generated by a linear spring
 * with either a zero or a non-zero resting length.
 *
 * \note This is the default force generation function employed by
 * class IBSpringForceGen.  It is associated with force_fcn_index 0.
 */
inline void
default_linear_spring_force(
    double F[NDIM],
    const double D[NDIM],
    const double& stf,
    const double& rst,
    const int& lag_idx)
{
    (void) lag_idx;

    /*
     * Compute the distance between the "master" and "slave" nodes.
     */
    double r_sq = 0.0;
    for (int d = 0; d < NDIM; ++d)
    {
        r_sq += D[d]*D[d];
    }
    const double r = sqrt(r_sq);

    /*
     * Compute the force applied to the "master" node.
     */
    if (r > std::numeric_limits<double>::epsilon())
    {
        const double stf_scal = stf*(1.0-rst/r);
        for (int d = 0; d < NDIM; ++d)
        {
            F[d] = stf_scal*D[d];
        }
    }
    return;
}// default_linear_spring_force

/*!
 * \brief Class IBSpringForceGen is a concrete
 * IBLagrangianForceStrategy that computes the force generated by a
 * collection of linear or nonlienar springs.
 *
 * \see IBSpringForceSpec
 */
class IBSpringForceGen
    : public IBLagrangianForceStrategy
{
public:
    /*!
     * \brief Default constructor.
     */
    IBSpringForceGen(
        SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> input_db=NULL);

    /*!
     * \brief Virtual destructor.
     */
    virtual ~IBSpringForceGen();

    /*!
     * \brief Register a spring force specification function with the
     * force generator.
     *
     * These functions are employed to compute the force generated by
     * a particular spring for the specified displacement, spring
     * constant, rest length, and Lagrangian index.
     */
    void registerSpringForceFunction(
        const int force_fcn_index,
        void (*force_fcn)(double F[NDIM], const double D[NDIM], const double& stf, const double& rst, const int& lag_idx));

    /*!
     * \brief Setup the data needed to compute the spring forces on
     * the specified level of the patch hierarchy.
     */
    virtual void initializeLevelData(
        const SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
        const int level_number,
        const double init_data_time,
        const bool initial_time,
        const LDataManager* const lag_manager);

    /*!
     * \brief Compute the spring forces generated by the Lagrangian
     * structure on the specified level of the patch hierarchy.
     *
     * \note Nodal forces computed by this method are \em added to the
     * force vector.
     */
    virtual void computeLagrangianForce(
        SAMRAI::tbox::Pointer<LNodeLevelData> F_data,
        SAMRAI::tbox::Pointer<LNodeLevelData> X_data,
        const SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
        const int level_number,
        const double data_time,
        const LDataManager* const lag_manager);

private:
    /*!
     * \brief Copy constructor.
     *
     * \note This constructor is not implemented and should not be
     * used.
     *
     * \param from The value to copy to this object.
     */
    IBSpringForceGen(
        const IBSpringForceGen& from);

    /*!
     * \brief Assignment operator.
     *
     * \note This operator is not implemented and should not be used.
     *
     * \param that The value to assign to this object.
     *
     * \return A reference to this object.
     */
    IBSpringForceGen& operator=(
        const IBSpringForceGen& that);

    /*!
     * \brief Read input values, indicated above, from given database.
     *
     * When assertion checking is active, the database pointer must be
     * non-null.
     */
    void getFromInput(
        SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> db);

    /*!
     * \name Data maintained separately for each level of the patch
     * hierarchy.
     */
    //\{
    std::vector<Mat> d_D_mats;
    std::vector<std::vector<int> > d_lag_mastr_node_idxs, d_petsc_mastr_node_idxs, d_petsc_slave_node_idxs;
    std::vector<std::vector<int> > d_force_fcn_idxs;
    std::vector<std::vector<double> > d_stiffnesses, d_rest_lengths;
    std::vector<bool> d_is_initialized;
    //\}

    /*!
     * \brief Spring force functions.
     */
    std::map<int,void (*)(double F[NDIM], const double D[NDIM], const double& stf, const double& rst, const int& lag_idx)> d_force_fcn_map;
};
}// namespace IBAMR

/////////////////////////////// INLINE ///////////////////////////////////////

//#include <ibamr/IBSpringForceGen.I>

//////////////////////////////////////////////////////////////////////////////

#endif //#ifndef included_IBSpringForceGen