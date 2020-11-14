#include "UnitTest/AST/Advice/ASTAdviceUnitTests.hpp"

TEST_F(AdviceSetTest, GetFirstNextAdvice_ByGivenTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advisenum = 0;
    OBJKey objkey;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;
    ywb_uint64_t inodeids[Constant::DEFAULT_ADVISE_NUM] =
    {1, 2, 3, 4, 5, 6};

    adviceset = GetAdviceSet();
    ret = adviceset->GetFirstAdvice(Tier::TIER_2, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeids[advisenum++]);
        ret = adviceset->GetNextAdvice(Tier::TIER_2, &advice);
    }
    ASSERT_EQ(advisenum, (ywb_uint32_t)Constant::DEFAULT_ADVISE_NUM);
}

TEST_F(AdviceSetTest, GetFirstNextAdvice_ByGivenTierButTierEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet();
    ret = adviceset->GetFirstAdvice(Tier::TIER_1, &advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, GetFirstNextAdvice_OnlyOneTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet2();
    inodeid = Constant::DEFAULT_INODE + (3 * Constant::DEFAULT_ADVISE_NUM);
    ret = adviceset->GetFirstAdvice(&advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeid);
        ret = adviceset->GetNextAdvice(&advice);
        inodeid++;
    }

    ret = adviceset->GetNextAdvice(&advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, GetFirstNextAdvice_Tier1Empty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    OBJKey objkey;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;
    ywb_uint64_t inodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] =
        {13, 1, 14, 2, 15, 3, 16, 4, 5, 6};

    adviceset = GetAdviceSet();
    ret = adviceset->GetFirstAdvice(&advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeids[advicenum++]);
        ret = adviceset->GetNextAdvice(&advice);
    }
    ASSERT_EQ(advicenum, 2 * (Constant::DEFAULT_ADVISE_NUM) - 2);
}

TEST_F(AdviceSetTest, GetFirstNextAdvice_AllTierEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet3();
    ret = adviceset->GetFirstAdvice(&advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, GetFirstNextAdviceSatisfy_Tier1Empty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    OBJKey objkey;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;
    ywb_uint64_t inodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] =
            {1, 14, 15, 3, 16, 4, 5};

    adviceset = GetAdviceSet();
    ret = adviceset->GetFirstAdvice(&advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        if((2 == objkey.GetInodeId()) ||
                (6 == objkey.GetInodeId()) ||
                (13 == objkey.GetInodeId()))
        {
            advice->SetFlagComb(Advice::ADV_chosen);
        }

        ret = adviceset->GetNextAdvice(&advice);
    }

    ret = adviceset->GetFirstAdviceSatisfy(Advice::ADV_chosen, false, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeids[advicenum++]);
        ret = adviceset->GetNextAdviceSatisfy(Advice::ADV_chosen, false, &advice);
    }
    ASSERT_EQ(advicenum, 7);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, GetFirstNextAdviceSatisfy_Tier1EmptyReentry_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    OBJKey objkey;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;
    ywb_uint64_t inodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] = {2, 4};

    adviceset = GetAdviceSet();
    ret = adviceset->GetFirstAdvice(&advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        if((2 == objkey.GetInodeId()) || (4 == objkey.GetInodeId()))
        {
            advice->SetFlagComb(Advice::ADV_chosen);
        }

        ret = adviceset->GetNextAdvice(&advice);
    }

    ret = adviceset->GetFirstAdviceSatisfy(Advice::ADV_chosen, true, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeids[advicenum++]);
        ret = adviceset->GetNextAdviceSatisfy(Advice::ADV_chosen, true, &advice);
    }
    ASSERT_EQ(advicenum, 2);
    ASSERT_EQ(ret, YFS_ENOENT);

    advicenum = 0;
    ret = adviceset->GetFirstAdviceSatisfy(Advice::ADV_chosen, true, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeids[advicenum++]);
        ret = adviceset->GetNextAdviceSatisfy(Advice::ADV_chosen, true, &advice);
    }
    ASSERT_EQ(advicenum, 2);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, GetNextAdviceSatisfy_AllTierEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet3();
    ret = adviceset->GetFirstAdviceSatisfy(Advice::ADV_chosen, true, &advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, Destroy_OnlyOneTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet2();
    adviceset->Destroy();
    ret = adviceset->GetFirstAdvice(&advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, Destroy_Tier1Empty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet();
    adviceset->Destroy();
    ret = adviceset->GetFirstAdvice(&advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceSetTest, Destroy_AllTierEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;

    adviceset = GetAdviceSet3();
    ret = adviceset->GetFirstAdvice(&advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolAdviceTest, GetFirstNextAdvice_ByGivenTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    inodeid = Constant::DEFAULT_INODE + (1 * Constant::DEFAULT_ADVISE_NUM);
    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdvice(Tier::TIER_1,
            AdviceType::AT_ctr_hot, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advicenum++;
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeid++);
        ret = subpooladvice->GetNextAdvice(Tier::TIER_1,
                AdviceType::AT_ctr_hot, &advice);
    }
    ASSERT_EQ(advicenum, Constant::DEFAULT_ADVISE_NUM - 2);

    advicenum = 0;
    inodeid = Constant::DEFAULT_INODE + (2 * Constant::DEFAULT_ADVISE_NUM);
    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdvice(Tier::TIER_1,
            AdviceType::AT_ctr_cold, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advicenum++;
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), inodeid++);
        ret = subpooladvice->GetNextAdvice(Tier::TIER_1,
                AdviceType::AT_ctr_cold, &advice);
    }
    ASSERT_EQ(advicenum, Constant::DEFAULT_ADVISE_NUM - 2);
}

TEST_F(SubPoolAdviceTest, GetFirstNextAdvice_ByGivenTierButNoAdviceOfSuchType_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdvice(Tier::TIER_0,
            AdviceType::AT_ctr_hot, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advicenum++;
        ret = subpooladvice->GetNextAdvice(Tier::TIER_0,
                AdviceType::AT_ctr_hot, &advice);
    }
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(advicenum, 0);

    ret = subpooladvice->GetFirstAdvice(Tier::TIER_2,
            AdviceType::AT_ctr_cold, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advicenum++;
        ret = subpooladvice->GetNextAdvice(Tier::TIER_2,
                AdviceType::AT_ctr_cold, &advice);
    }
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(advicenum, 0);
}

TEST_F(SubPoolAdviceTest, GetFirstNextAdvice_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    OBJKey objkey;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    ywb_uint64_t hotinodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] =
            {7, 1, 8, 2, 9, 3, 10, 4, 5, 6};
    ywb_uint64_t coldinodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] =
            {19, 13, 20, 14, 21, 15, 22, 16, 23, 24};

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_hot, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), hotinodeids[advicenum++]);
        ret = subpooladvice->GetNextAdvice(AdviceType::AT_ctr_hot, &advice);
    }
    ASSERT_EQ(advicenum, 2 * (Constant::DEFAULT_ADVISE_NUM) - 2);

    advicenum = 0;
    ret = subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_cold, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), coldinodeids[advicenum++]);
        ret = subpooladvice->GetNextAdvice(AdviceType::AT_ctr_cold, &advice);
    }
    ASSERT_EQ(advicenum, 2 * (Constant::DEFAULT_ADVISE_NUM) - 2);
}

TEST_F(SubPoolAdviceTest, GetFirstNextAdvice_NoAdviceOfGivenType_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdvice(AdviceType::AT_swap_down, &advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolAdviceTest, GetFirstNextAdviceSatisfy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t advicenum = 0;
    OBJKey objkey;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    ywb_uint64_t hotinodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] =
            {1, 6};
    ywb_uint64_t coldinodeids[2 * Constant::DEFAULT_ADVISE_NUM - 2] =
            {23};

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_hot, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        if((1 == objkey.GetInodeId()) || (6 == objkey.GetInodeId()))
        {
            advice->SetFlagComb(Advice::ADV_chosen);
        }

        ret = subpooladvice->GetNextAdvice(AdviceType::AT_ctr_hot, &advice);
    }

    ret = subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_cold, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        if((23 == objkey.GetInodeId()))
        {
            advice->SetFlagComb(Advice::ADV_chosen);
        }

        ret = subpooladvice->GetNextAdvice(AdviceType::AT_ctr_cold, &advice);
    }

    ret = subpooladvice->GetFirstAdviceSatisfy(
            AdviceType::AT_ctr_hot, Advice::ADV_chosen, true, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), hotinodeids[advicenum++]);
        ret = subpooladvice->GetNextAdviceSatisfy(
                AdviceType::AT_ctr_hot, Advice::ADV_chosen, true, &advice);
    }
    ASSERT_EQ(advicenum, 2);

    advicenum = 0;
    ret = subpooladvice->GetFirstAdviceSatisfy(
            AdviceType::AT_ctr_cold, Advice::ADV_chosen, true, &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        ASSERT_EQ(objkey.GetInodeId(), coldinodeids[advicenum++]);
        ret = subpooladvice->GetNextAdviceSatisfy(
                AdviceType::AT_ctr_cold, Advice::ADV_chosen, true, &advice);
    }
    ASSERT_EQ(advicenum, 1);
}

TEST_F(SubPoolAdviceTest, GetFirstNextAdviceSatisfy_NoAdviceOfGivenType_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->GetFirstAdviceSatisfy(
            AdviceType::AT_swap_down, Advice::ADV_chosen, false, &advice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolAdviceTest, IsFirstTimeToGetFrom_BeforeCallGetFirstAdvice_TRUE)
{
    ywb_bool_t ret = false;
    SubPoolAdvice* subpooladvice = NULL;

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->IsFirstTimeToGetFromTierBasedAdvice(AdviceType::AT_ctr_hot);
    ASSERT_EQ(ret, true);
}

TEST_F(SubPoolAdviceTest, IsFirstTimeToGetFrom_AfterCallGetFirstAdvice_FALSE)
{
    ywb_bool_t ret = false;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    subpooladvice = GetSubPoolAdvice();
    subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_hot, &advice);
    ret = subpooladvice->IsFirstTimeToGetFromTierBasedAdvice(AdviceType::AT_ctr_hot);
    ASSERT_EQ(ret, false);
}

TEST_F(SubPoolAdviceTest, IsFirstTimeToGetFrom_AfterCallGetNextAdvice_FALSE)
{
    ywb_bool_t ret = false;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    subpooladvice = GetSubPoolAdvice();
    subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_hot, &advice);
    subpooladvice->GetNextAdvice(AdviceType::AT_ctr_hot, &advice);
    ret = subpooladvice->IsFirstTimeToGetFromTierBasedAdvice(AdviceType::AT_ctr_hot);
    ASSERT_EQ(ret, false);
}

TEST_F(SubPoolAdviceTest, IsFirstTimeToGetFrom_NoAdviceOfGivenType_TRUE)
{
    ywb_bool_t ret = false;
    SubPoolAdvice* subpooladvice = NULL;

    subpooladvice = GetSubPoolAdvice();
    ret = subpooladvice->IsFirstTimeToGetFromTierBasedAdvice(AdviceType::AT_swap_down);
    ASSERT_EQ(ret, true);
}

TEST_F(AdviceManagerTest, GetSubPoolAdvice_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    AdviceManager* advicemgr = NULL;
    SubPool* subpool = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    ywb_uint32_t tiercombs[7] = {SubPool::TC_ssd_ent_sata,
            SubPool::TC_ssd_ent, SubPool::TC_ssd_sata,
            SubPool::TC_ent_sata, SubPool::TC_ssd,
            SubPool::TC_ent, SubPool::TC_sata};

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    advicemgr = GetAdviceManager();

    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
        ASSERT_EQ(subpooladvice->GetTierComb(), tiercombs[subpoolnum++]);
        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }
    ASSERT_EQ(subpoolnum, 7);
}

TEST_F(AdviceManagerTest, RemoveSubPoolAdvice_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    list<SubPoolKey> subpoolkeys;
    list<SubPool*> subpools;
    list<SubPoolKey>::iterator subpoolkeyiter;

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    advicemgr = GetAdviceManager();

    config->GetKeysAndSubPools(subpoolkeys, subpools);
    subpoolkeyiter = subpoolkeys.begin();
    while(subpoolkeyiter != subpoolkeys.end())
    {
        subpoolkey = *subpoolkeyiter;
        advicemgr->RemoveSubPoolAdvice(subpoolkey);
        subpoolkeyiter++;
    }

    subpoolkeyiter = subpoolkeys.begin();
    while(subpoolkeyiter != subpoolkeys.end())
    {
        subpoolkey = *subpoolkeyiter;
        advicemgr->RemoveSubPoolAdvice(subpoolkey);
        ret = advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
        ASSERT_EQ(ret, YFS_ENOENT);
        subpoolkeyiter++;
    }
}

TEST_F(AdviceManagerTest, CalculateTheoreticalAdviceNum_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    ywb_uint32_t tier = 0;
    ywb_uint32_t numabove = 0;
    ywb_uint32_t numbelow = 0;
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    AdviceManager* advicemgr = NULL;
    SubPool* subpool = NULL;
    /*
     * NOTE: this should keep consistent with AdviceManagerTest::Setup()
     * and AdviceManager::CalculateTheoreticalAdviceNum()
     * */
    ywb_uint32_t migratableforssd =
            (((Constant::DEFAULT_SSD_BW * 100) / (Constant::DEFAULT_OBJ_SIZE)) *
                    (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR));
    ywb_uint32_t migratableforent =
            (((Constant::DEFAULT_ENT_BW * 100) / (Constant::DEFAULT_OBJ_SIZE)) *
                    (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR));
    ywb_uint32_t migratableforsata =
            (((Constant::DEFAULT_SATA_BW * 100) / (Constant::DEFAULT_OBJ_SIZE)) *
                (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR));
    ywb_uint32_t numdistrib[7][3][AdviceType::AT_cnt] = {
            {{0, migratableforssd, /*0, 0, 0, */0, 0}, {migratableforent, migratableforent, /*0, 0, 0, */0, 0}, {migratableforsata, 0, /*0, 0, 0, */0, 0}},
            {{0, migratableforssd, /*0, 0, 0, */0, 0}, {migratableforent, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}},
            {{0, migratableforssd, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}, {migratableforsata, 0, /*0, 0, 0, */0, 0}},
            {{0, 0, /*0, 0, 0, */0, 0}, {0, migratableforent, /*0, 0, 0, */0, 0}, {migratableforsata, 0, /*0, 0, 0, */0, 0}},
            {{0, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}},
            {{0, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}},
            {{0, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}, {0, 0, /*0, 0, 0, */0, 0}},
    };

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    advicemgr = GetAdviceManager();

    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
        {
            advicemgr->CalculateTheoreticalAdviceNum(subpoolkey,
                    tier, AdviceType::AT_ctr_hot, &numabove, &numbelow);
            ASSERT_EQ(numabove, numdistrib[subpoolnum][tier][AdviceType::AT_ctr_hot]);
            ASSERT_EQ(numbelow, numdistrib[subpoolnum][tier][AdviceType::AT_ctr_hot]);
            advicemgr->CalculateTheoreticalAdviceNum(subpoolkey,
                    tier, AdviceType::AT_ctr_cold, &numabove, &numbelow);
            ASSERT_EQ(numbelow, numdistrib[subpoolnum][tier][AdviceType::AT_ctr_cold]);
            ASSERT_EQ(numabove, numdistrib[subpoolnum][tier][AdviceType::AT_ctr_cold]);
        }

        subpoolnum++;
        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }
}

TEST_F(AdviceManagerTest, AdoptAdviceRule_ThreeTiers_SUCCESS)
{
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule rule;
    AdviceManager* advicemgr = NULL;
    HeatBucket* hotheatbucket = NULL;
    HeatBucket* coldheatbucket = NULL;
    ywb_uint64_t hotadvicesequence[3][6] =
            {{0, 0, 0, 0}, {12, 11, 10, 9}, {18, 17, 16, 15}};
    ywb_uint64_t coldadvicesequence[3][6] =
            {{4, 3}, {10, 9}, {0, 0}};
    ywb_uint32_t hotadvicenum[3] = {0, 4, 4};
    ywb_uint32_t coldadvicenum[3] = {2, 2, 0};

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(rule);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
        match = CheckHeatBucketAdviceSequence(subpoolkey, tier, rule,
                hotadvicesequence[tier], hotadvicenum[tier]);
        ASSERT_EQ(match, true);
    }
    hotheatbucket = GetHeatBucket(subpoolkey, Tier::TIER_1, rule);

    BuildDefaultColdAdviceRule(rule);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
        match = CheckHeatBucketAdviceSequence(subpoolkey, tier, rule,
                coldadvicesequence[tier], coldadvicenum[tier]);
        ASSERT_EQ(match, true);
    }
    coldheatbucket = GetHeatBucket(subpoolkey, Tier::TIER_1, rule);

    ASSERT_EQ(hotheatbucket != coldheatbucket, true);
}

TEST_F(AdviceManagerTest, AdoptAdviceRule_NoTier1_SUCCESS)
{
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule rule;
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[3][6] =
            {{0, 0, 0, 0}, {0, 0, 0, 0}, {54, 53, 52, 51}};
    ywb_uint64_t coldadvicesequence[3][6] =
            {{40, 39}, {0, 0}, {0, 0}};
    ywb_uint32_t hotadvicenum[3] = {0, 0, 4};
    ywb_uint32_t coldadvicenum[3] = {2, 0, 0};

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 2);

    BuildDefaultHotAdviceRule(rule);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
        match = CheckHeatBucketAdviceSequence(subpoolkey, tier, rule,
                hotadvicesequence[tier], hotadvicenum[tier]);
        ASSERT_EQ(match, true);
    }

    BuildDefaultColdAdviceRule(rule);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
        match = CheckHeatBucketAdviceSequence(subpoolkey, tier, rule,
                coldadvicesequence[tier], coldadvicenum[tier]);
        ASSERT_EQ(match, true);
    }
}

TEST_F(AdviceManagerTest, AdoptAdviceRule_SameRuleForBothHotAndCold_SUCCESS)
{
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule rule;
    AdviceManager* advicemgr = NULL;
    HeatBucket* hotheatbucket = NULL;
    HeatBucket* coldheatbucket = NULL;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(rule);
    tier = Tier::TIER_1;
    advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
    hotheatbucket = GetHeatBucket(subpoolkey, tier, rule);

    rule.SetAdviceType(AdviceType::AT_ctr_cold);
    advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
    coldheatbucket = GetHeatBucket(subpoolkey, tier, rule);

    ASSERT_EQ(hotheatbucket == coldheatbucket, true);
}

TEST_F(AdviceManagerTest, AdoptAdviceRule_SameRuleAgain_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule rule;
    AdviceManager* advicemgr = NULL;
    HeatBucket* prevheatbucket = NULL;
    HeatBucket* curheatbucket = NULL;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(rule);
    tier = Tier::TIER_1;
    advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
    prevheatbucket = GetHeatBucket(subpoolkey, tier, rule);

    ret = advicemgr->AdoptAdviceRule(subpoolkey, tier, rule);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curheatbucket = GetHeatBucket(subpoolkey, tier, rule);
    ASSERT_EQ(prevheatbucket == curheatbucket, true);
}

TEST_F(AdviceManagerTest, AdoptFilterRule_SUCCESS)
{
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule rule1;
    AdviceRule rule2;
    AdviceRule filterrule[2];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[3][6] =
            {{0, 0, 0, 0}, {12, 11, 10, 9}, {18, 17, 16, 15}};
    ywb_uint64_t coldadvicesequence[3][6] =
            {{0, 0}, {10, 9}, {0, 0}};
    ywb_uint32_t hotadvicenum[3] = {0, 4, 4};
    ywb_uint32_t coldadvicenum[3] = {0, 2, 0};

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(rule1);
    BuildDefaultColdAdviceRule(rule2);
    filterrule[0] = rule1;
    filterrule[1] = rule2;
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        /*
         * NOTE:
         * AdoptFilterRule will stop if AdoptAdviceRule on one
         * filter rule fails, so those successive filter rules
         * will not be executed
         **/
        advicemgr->AdoptFilterRule(subpoolkey, tier, filterrule, 2);
        match = CheckHeatBucketAdviceSequence(subpoolkey, tier, filterrule[0],
                hotadvicesequence[tier], hotadvicenum[tier]);
        ASSERT_EQ(match, true);
        match = CheckHeatBucketAdviceSequence(subpoolkey, tier, filterrule[1],
                coldadvicesequence[tier], coldadvicenum[tier]);
        ASSERT_EQ(match, true);
    }
}

TEST_F(AdviceManagerTest, IntersectAdviceAndFilterRule_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t advicesequence[2] = {10, 9};
    ywb_uint32_t advicenum = 2;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    tier = Tier::TIER_1;
    advicemgr->AdoptAdviceRule(subpoolkey, tier, advicerule);
    advicemgr->AdoptFilterRule(subpoolkey, tier, filterrule, 1);
    ret = advicemgr->IntersectAdviceAndFilterRule(
            subpoolkey, tier, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, advicesequence, advicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, IntersectAdviceAndFilterRule_FilterRuleNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[8] = {12, 18, 11, 17, 10, 16, 9, 15};
    ywb_uint32_t hotadvicenum = 8;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        ret = advicemgr->AdoptAdviceRule(subpoolkey, tier, advicerule);
        if(YWB_SUCCESS != ret)
        {
            continue;
        }

        ret = advicemgr->AdoptFilterRule(subpoolkey, tier, NULL, 0);
        if(YWB_SUCCESS != ret)
        {
            continue;
        }

        ret = advicemgr->IntersectAdviceAndFilterRule(
                subpoolkey, tier, advicerule, NULL, 0);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }

    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, hotadvicesequence, hotadvicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, PreGenerateCheck_SubPoolHasNoTier_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceManager* advicemgr = NULL;
    SubPool* subpool = NULL;

    advicemgr = GetAdviceManager();
    subpool = BuildSubPoolWithoutTier();
    ret = advicemgr->PreGenerateCheck(subpool);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_disk), true);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_obj), true);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_advice), true);
    DestroySubPool(subpool);
}

TEST_F(AdviceManagerTest, PreGenerateCheck_SubPoolHasTierButNoOBJ_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiertype = 0;
    AdviceManager* advicemgr = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;

    advicemgr = GetAdviceManager();
    subpool = BuildSubPoolWithTierButNoOBJ();
    ret = advicemgr->PreGenerateCheck(subpool);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_disk), false);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_obj), true);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_advice), true);
    for(tiertype = Tier::TIER_0; tiertype < Tier::TIER_cnt; tiertype++)
    {
        ret = subpool->GetTier(tiertype, &tier);
        if((YWB_SUCCESS == ret) && (tier != NULL))
        {
            ASSERT_EQ(tier->TestFlag(Tier::TF_no_disk), false);
            ASSERT_EQ(tier->TestFlag(Tier::TF_no_obj), true);
            ASSERT_EQ(tier->TestFlag(Tier::TF_no_advice), true);
        }
    }

    DestroySubPool(subpool);
}

TEST_F(AdviceManagerTest, PreGenerateCheck_SubPoolHasTierAndOBJs_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiertype = 0;
    AdviceManager* advicemgr = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;

    advicemgr = GetAdviceManager();
    subpool = BuildSubPoolWithTierAndOBJs();
    ret = advicemgr->PreGenerateCheck(subpool);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_disk), false);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_obj), false);
    ASSERT_EQ(subpool->TestFlag(SubPool::SPF_no_advice), true);
    for(tiertype = Tier::TIER_0; tiertype < Tier::TIER_cnt; tiertype++)
    {
        ret = subpool->GetTier(tiertype, &tier);
        if((YWB_SUCCESS == ret) && (tier != NULL))
        {
            ASSERT_EQ(tier->TestFlag(Tier::TF_no_disk), false);
            if(0 == tier->GetOBJCnt())
            {
                ASSERT_EQ(tier->TestFlag(Tier::TF_no_obj), true);
            }
            else
            {
                ASSERT_EQ(tier->TestFlag(Tier::TF_no_obj), false);
            }
            ASSERT_EQ(tier->TestFlag(Tier::TF_no_advice), true);
        }
    }

    DestroySubPool(subpool);
}

TEST_F(AdviceManagerTest, GenerateAdvice_WithTierSpecified_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t advicesequence[2] = {10, 9};
    ywb_uint32_t advicenum = 2;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    tier = Tier::TIER_1;
    ret = advicemgr->GenerateAdvice(subpoolkey,
            tier, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, advicesequence, advicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_WithTierSpecifiedButAdoptAdviceRuleFail_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    tier = Tier::TIER_0;
    ret = advicemgr->GenerateAdvice(subpoolkey,
            tier, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_ENODATA);
    match = CheckSubPoolAdviceSequence(subpoolkey, advicerule, NULL, 0);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_WithTierSpecifiedButAdoptFilterRuleFail_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    ywb_uint32_t tier = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    /*subpool of tier combination: ssd + ent + sata*/
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultColdAdviceRule(rule);
    filterrule[0] = rule;

    tier = Tier::TIER_2;
    ret = advicemgr->GenerateAdvice(subpoolkey,
            tier, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_ENODATA);
    match = CheckSubPoolAdviceSequence(subpoolkey, advicerule, NULL, 0);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceManagerTest, GenerateAdvice_FilterRuleNumExceed_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[10];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 10);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(AdviceManagerTest, GenerateAdvice_AdviceRuleInvalid_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    advicerule.SetPartitionBase(PartitionBase::PB_none);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(AdviceManagerTest, GenerateAdvice_FilterRuleInvalid_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    rule.SetSortBase(SortBase::SB_none);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(AdviceManagerTest, GenerateAdvice_AdviceRuleAndFilterRuleMismatch_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultColdAdviceRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier012_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[4] = {10, 16, 9, 15};
    ywb_uint32_t hotadvicenum = 4;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, hotadvicesequence, hotadvicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier01_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[2] = {28, 27};
    ywb_uint32_t hotadvicenum = 2;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, hotadvicesequence, hotadvicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier02_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[2] = {52, 51};
    ywb_uint32_t hotadvicenum = 2;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 2);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, hotadvicesequence, hotadvicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier12_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    ywb_uint64_t hotadvicesequence[2] = {70, 69};
    ywb_uint32_t hotadvicenum = 2;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 3);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, hotadvicesequence, hotadvicenum);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier0_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 4);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_ENODATA);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, NULL, 0);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier1_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 5);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_ENODATA);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, NULL, 0);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, GenerateAdvice_SubPoolWithTier2_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    ret = advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ASSERT_EQ(ret, YFS_ENODATA);
    match = CheckSubPoolAdviceSequence(subpoolkey,
            advicerule, NULL, 0);
    ASSERT_EQ(match, true);
}

TEST_F(AdviceManagerTest, Destroy_WithSubPoolSpecified_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceManager* advicemgr = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);

    ret = advicemgr->Destroy(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceManagerTest, Destroy_WithSubPoolSpecified_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    ret = advicemgr->Destroy(subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(AdviceManagerTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    }

    ret = advicemgr->Destroy();
    ASSERT_EQ(ret, YWB_SUCCESS);
    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        ret = advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(AdviceManagerTest, Reset_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    SubPoolKey subpoolkey;
    AdviceRule advicerule;
    AdviceRule rule;
    AdviceRule filterrule[1];
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);

    BuildDefaultHotAdviceRule(advicerule);
    BuildDefaultHotFilterRule(rule);
    filterrule[0] = rule;

    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        advicemgr->GenerateAdvice(subpoolkey, advicerule, filterrule, 1);
    }

    ret = advicemgr->Reset();
    ASSERT_EQ(ret, YWB_SUCCESS);
    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        ret = advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(AdviceManagerTest, Init_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    SubPoolKey subpoolkey;
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    advicemgr = GetAdviceManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);

    /*Init() already been called in AdviceManagerTest::Setup()*/
    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        ret = advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = subpooladvice->GetFirstAdvice(AdviceType::AT_ctr_hot, &advice);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

/* vim:set ts=4 sw=4 expandtab */
