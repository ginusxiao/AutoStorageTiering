#ifndef __AST_ERROR_HPP__
#define __AST_ERROR_HPP__

#include "stdlib.h"

class AST;
class Error
{
public:
    /*define the reason caused the error*/
    enum Code
    {
        /*COMMON PART*/
        COM_success = 0,
        COME_out_of_memory,

        /*AST SPECIFIC PART*/
        ASTE_start = 100,

        /*TIME SPECIFIC PART*/
        TIME_start = 200,

        /*CONFIG SPECIFIC PART*/
        CFGE_start = 300,

        /*DISK PROFILE SPECIFIC PART*/
        DPFE_start = 400,

        /*RELOCATION SPECIFIC PART*/
        RLCE_start = 500,

        /*LOGICAL CONFIG SPECIFIC PART*/
        LCFE_start = 600,
        LCFE_obj_not_exist,
        LCFE_source_disk_not_exist,
        LCFE_target_disk_not_exist,
        LCFE_subpool_not_exist,

        /*IO SPECIFIC PART*/
        IOME_start = 700,

        /*HEAT DISTRIBUTION SPECIFIC PART*/
        HDTE_start = 800,

        /*ADVICE SPECIFIC PART*/
        ADVE_start = 900,

        /*DISK PERFORMANCE SPECIFIC PART*/
        DPME_start = 1000,

        /*PLAN SPECIFIC PART*/
        PLNE_start = 1100,
        PLNE_subpool_no_more_plan,
        PLNE_subpool_schedule_timed_delay,


        /*ARB SPECIFIC PART*/
        ARBE_start = 1200,
        ARBE_conflict,
        ARBE_frequent_up_and_down,
        ARBE_benefit_less_than_cost,

        /*MIGRATION SPECIFIC PART*/
        MIGE_start = 1300,

        /*MESSAGE SPECIFIC PART*/
        MSGE_start = 1400,

        /*GENERATOR SPECIFIC PART*/
        GENE_start = 1500,
        GENE_fail_to_start,

        /*EXECUTOR SPECIFIC PART*/
        EXEE_start = 1600,

    };

    Error(AST* ast)
    {
        mAst = ast;
        Reset();
    }

    ~Error()
    {
        mAst = NULL;
    }

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    inline Error::Code GetASTErr()
    {
        return mASTErr;
    }

    inline Error::Code GetTIMErr()
    {
        return mTIMErr;
    }

    inline Error::Code GetCFGErr()
    {
        return mCFGErr;
    }

    inline Error::Code GetDPFErr()
    {
        return mDPFErr;
    }

    inline Error::Code GetRLCErr()
    {
        return mRLCErr;
    }

    inline Error::Code GetLCFErr()
    {
        return mLCFErr;
    }

    inline Error::Code GetIOMErr()
    {
        return mIOMErr;
    }

    inline Error::Code GetHDTErr()
    {
        return mHDTErr;
    }

    inline Error::Code GetADVErr()
    {
        return mADVErr;
    }

    inline Error::Code GetDPMErr()
    {
        return mDPMErr;
    }

    inline Error::Code GetPLNErr()
    {
        return mPLNErr;
    }

    inline Error::Code GetARBErr()
    {
        return mARBErr;
    }

    inline Error::Code GetMIGErr()
    {
        return mMIGErr;
    }

    inline Error::Code GetMSGErr()
    {
        return mMSGErr;
    }

    inline Error::Code GetGENErr()
    {
        return mGENErr;
    }

    inline Error::Code GetEXEErr()
    {
        return mEXEErr;
    }

    inline void SetASTErr(Error::Code err)
    {
        mASTErr = err;
    }

    inline void SetTIMErr(Error::Code err)
    {
        mTIMErr = err;
    }

    inline void SetCFGErr(Error::Code err)
    {
        mCFGErr = err;
    }

    inline void SetDPFErr(Error::Code err)
    {
        mDPFErr = err;
    }

    inline void SetRLCErr(Error::Code err)
    {
        mRLCErr = err;
    }

    inline void SetLCFErr(Error::Code err)
    {
        mLCFErr = err;
    }

    inline void SetIOMErr(Error::Code err)
    {
        mIOMErr = err;
    }

    inline void SetHDTErr(Error::Code err)
    {
        mHDTErr = err;
    }

    inline void SetADVErr(Error::Code err)
    {
        mADVErr = err;
    }

    inline void SetDPMErr(Error::Code err)
    {
        mDPMErr = err;
    }

    inline void SetPLNErr(Error::Code err)
    {
        mPLNErr = err;
    }

    inline void SetARBErr(Error::Code err)
    {
        mARBErr = err;
    }

    inline void SetMIGErr(Error::Code err)
    {
        mMIGErr = err;
    }

    inline void SetMSGErr(Error::Code err)
    {
        mMSGErr = err;
    }

    inline void SetGENErr(Error::Code err)
    {
        mGENErr = err;
    }

    inline void SetEXEErr(Error::Code err)
    {
        mEXEErr = err;
    }

    inline void Reset()
    {
        mASTErr = Error::COM_success;
        mTIMErr = Error::COM_success;
        mCFGErr = Error::COM_success;
        mDPFErr = Error::COM_success;
        mRLCErr = Error::COM_success;
        mLCFErr = Error::COM_success;
        mIOMErr = Error::COM_success;
        mHDTErr = Error::COM_success;
        mADVErr = Error::COM_success;
        mDPMErr = Error::COM_success;
        mPLNErr = Error::COM_success;
        mARBErr = Error::COM_success;
        mMIGErr = Error::COM_success;
        mMSGErr = Error::COM_success;

        mGENErr = Error::COM_success;
        mEXEErr = Error::COM_success;
    }

private:
    AST* mAst;
    /*
     * #CALL STATCK#
     * Routine A calls routine B, routine B calls routine C and so on,
     * then all those routines together form a call stack, and A is
     * upper layer of B, B is lower layer of A...
     *
     * Sometimes, upper layer routine's successive action is dependent
     * on lower layer routine's action.
     *
     * #ERROR CODE FOR MODULE INTERNAL USAGE#
     * If upper layer routine and lower layer routine are in the same
     * module, we name it module internal invoking, and error code for
     * lower layer is for module internal usage.
     *
     * #ERROR CODE FOR THREAD INTERNAL USAGE#
     * If upper layer routine and lower layer routine are within the same
     * thread, we name it thread internal invoking, and error code for
     * lower layer is for thread internal usage.
     *
     * Currently, ONLY error code for thread internal usage is used.
     **/

     /*
     * Error code for every module, mainly for module internally usage.
     *
     * One module must call SetErrCode() to set current error code,
     * one module must call GetErrCode() to load current error code.
     *
     * NOTE: one module can have error code of other module, take
     * plan module as an example, it can have error code of logical
     * configuration module, such as LCF_EOBJNOTEXIST.
     **/
    Error::Code mASTErr;
    Error::Code mTIMErr;
    Error::Code mCFGErr;
    Error::Code mDPFErr;
    Error::Code mRLCErr;
    Error::Code mLCFErr;
    Error::Code mIOMErr;
    Error::Code mHDTErr;
    Error::Code mADVErr;
    Error::Code mDPMErr;
    Error::Code mPLNErr;
    Error::Code mARBErr;
    Error::Code mMIGErr;
    Error::Code mMSGErr;

    /*
     * error code for thread, mainly for thread internal usage.
     **/
    Error::Code mGENErr;
    Error::Code mEXEErr;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
