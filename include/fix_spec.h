#pragma once

#include <unordered_map>
#include <string>

#define FIX_VER_MAJOR 4
#define FIX_VER_MINOR 4

namespace fix_spec {
	enum tag: int {
		Account = 1,
		AdvId = 2,
		AdvRefID = 3,
		AdvSide = 4,
		AdvTransType = 5,
		AvgPx = 6,
		BeginSeqNo = 7,
		BeginString = 8,
		BodyLength = 9,
		CheckSum = 10,
		ClOrdID = 11,
		Commission = 12,
		CommType = 13,
		CumQty = 14,
		Currency = 15,
		EndSeqNo = 16,
		ExecID = 17,
		ExecInst = 18,
		ExecRefID = 19,
		HandlInst = 21,
		SecurityIDSource = 22,
		IOIID = 23,
		IOIQltyInd = 25,
		IOIRefID = 26,
		IOIQty = 27,
		IOITransType = 28,
		LastCapacity = 29,
		LastMkt = 30,
		LastPx = 31,
		LastQty = 32,
		LinesOfText = 33,
		MsgSeqNum = 34,
		MsgType = 35,
		NewSeqNo = 36,
		OrderID = 37,
		OrderQty = 38,
		OrdStatus = 39,
		OrdType = 40,
		OrigClOrdID = 41,
		OrigTime = 42,
		PossDupFlag = 43,
		Price = 44,
		RefSeqNum = 45,
		SecurityID = 48,
		SenderCompID = 49,
		SenderSubID = 50,
		SendingTime = 52,
		Quantity = 53,
		Side = 54,
		Symbol = 55,
		TargetCompID = 56,
		TargetSubID = 57,
		Text = 58,
		TimeInForce = 59,
		TransactTime = 60,
		Urgency = 61,
		ValidUntilTime = 62,
		SettlType = 63,
		SettlDate = 64,
		SymbolSfx = 65,
		ListID = 66,
		ListSeqNo = 67,
		TotNoOrders = 68,
		ListExecInst = 69,
		AllocID = 70,
		AllocTransType = 71,
		RefAllocID = 72,
		NoOrders = 73,
		AvgPxPrecision = 74,
		TradeDate = 75,
		PositionEffect = 77,
		NoAllocs = 78,
		AllocAccount = 79,
		AllocQty = 80,
		ProcessCode = 81,
		NoRpts = 82,
		RptSeq = 83,
		CxlQty = 84,
		NoDlvyInst = 85,
		AllocStatus = 87,
		AllocRejCode = 88,
		Signature = 89,
		SecureDataLen = 90,
		SecureData = 91,
		SignatureLength = 93,
		EmailType = 94,
		RawDataLength = 95,
		RawData = 96,
		PossResend = 97,
		EncryptMethod = 98,
		StopPx = 99,
		ExDestination = 100,
		CxlRejReason = 102,
		OrdRejReason = 103,
		IOIQualifier = 104,
		WaveNo = 105,
		Issuer = 106,
		SecurityDesc = 107,
		HeartBtInt = 108,
		MinQty = 110,
		MaxFloor = 111,
		TestReqID = 112,
		ReportToExch = 113,
		LocateReqd = 114,
		OnBehalfOfCompID = 115,
		OnBehalfOfSubID = 116,
		QuoteID = 117,
		NetMoney = 118,
		SettlCurrAmt = 119,
		SettlCurrency = 120,
		ForexReq = 121,
		OrigSendingTime = 122,
		GapFillFlag = 123,
		NoExecs = 124,
		ExpireTime = 126,
		DKReason = 127,
		DeliverToCompID = 128,
		DeliverToSubID = 129,
		IOINaturalFlag = 130,
		QuoteReqID = 131,
		BidPx = 132,
		OfferPx = 133,
		BidSize = 134,
		OfferSize = 135,
		NoMiscFees = 136,
		MiscFeeAmt = 137,
		MiscFeeCurr = 138,
		MiscFeeType = 139,
		PrevClosePx = 140,
		ResetSeqNumFlag = 141,
		SenderLocationID = 142,
		TargetLocationID = 143,
		OnBehalfOfLocationID = 144,
		DeliverToLocationID = 145,
		NoRelatedSym = 146,
		Subject = 147,
		Headline = 148,
		URLLink = 149,
		ExecType = 150,
		LeavesQty = 151,
		CashOrderQty = 152,
		AllocAvgPx = 153,
		AllocNetMoney = 154,
		SettlCurrFxRate = 155,
		SettlCurrFxRateCalc = 156,
		NumDaysInterest = 157,
		AccruedInterestRate = 158,
		AccruedInterestAmt = 159,
		SettlInstMode = 160,
		AllocText = 161,
		SettlInstID = 162,
		SettlInstTransType = 163,
		EmailThreadID = 164,
		SettlInstSource = 165,
		SecurityType = 167,
		EffectiveTime = 168,
		StandInstDbType = 169,
		StandInstDbName = 170,
		StandInstDbID = 171,
		SettlDeliveryType = 172,
		BidSpotRate = 188,
		BidForwardPoints = 189,
		OfferSpotRate = 190,
		OfferForwardPoints = 191,
		OrderQty2 = 192,
		SettlDate2 = 193,
		LastSpotRate = 194,
		LastForwardPoints = 195,
		AllocLinkID = 196,
		AllocLinkType = 197,
		SecondaryOrderID = 198,
		NoIOIQualifiers = 199,
		MaturityMonthYear = 200,
		PutOrCall = 201,
		StrikePrice = 202,
		CoveredOrUncovered = 203,
		OptAttribute = 206,
		SecurityExchange = 207,
		NotifyBrokerOfCredit = 208,
		AllocHandlInst = 209,
		MaxShow = 210,
		PegOffsetValue = 211,
		XmlDataLen = 212,
		XmlData = 213,
		SettlInstRefID = 214,
		NoRoutingIDs = 215,
		RoutingType = 216,
		RoutingID = 217,
		Spread = 218,
		BenchmarkCurveCurrency = 220,
		BenchmarkCurveName = 221,
		BenchmarkCurvePoint = 222,
		CouponRate = 223,
		CouponPaymentDate = 224,
		IssueDate = 225,
		RepurchaseTerm = 226,
		RepurchaseRate = 227,
		Factor = 228,
		TradeOriginationDate = 229,
		ExDate = 230,
		ContractMultiplier = 231,
		NoStipulations = 232,
		StipulationType = 233,
		StipulationValue = 234,
		YieldType = 235,
		Yield = 236,
		TotalTakedown = 237,
		Concession = 238,
		RepoCollateralSecurityType = 239,
		RedemptionDate = 240,
		UnderlyingCouponPaymentDate = 241,
		UnderlyingIssueDate = 242,
		UnderlyingRepoCollateralSecurityType = 243,
		UnderlyingRepurchaseTerm = 244,
		UnderlyingRepurchaseRate = 245,
		UnderlyingFactor = 246,
		UnderlyingRedemptionDate = 247,
		LegCouponPaymentDate = 248,
		LegIssueDate = 249,
		LegRepoCollateralSecurityType = 250,
		LegRepurchaseTerm = 251,
		LegRepurchaseRate = 252,
		LegFactor = 253,
		LegRedemptionDate = 254,
		CreditRating = 255,
		UnderlyingCreditRating = 256,
		LegCreditRating = 257,
		TradedFlatSwitch = 258,
		BasisFeatureDate = 259,
		BasisFeaturePrice = 260,
		MDReqID = 262,
		SubscriptionRequestType = 263,
		MarketDepth = 264,
		MDUpdateType = 265,
		AggregatedBook = 266,
		NoMDEntryTypes = 267,
		NoMDEntries = 268,
		MDEntryType = 269,
		MDEntryPx = 270,
		MDEntrySize = 271,
		MDEntryDate = 272,
		MDEntryTime = 273,
		TickDirection = 274,
		MDMkt = 275,
		QuoteCondition = 276,
		TradeCondition = 277,
		MDEntryID = 278,
		MDUpdateAction = 279,
		MDEntryRefID = 280,
		MDReqRejReason = 281,
		MDEntryOriginator = 282,
		LocationID = 283,
		DeskID = 284,
		DeleteReason = 285,
		OpenCloseSettlFlag = 286,
		SellerDays = 287,
		MDEntryBuyer = 288,
		MDEntrySeller = 289,
		MDEntryPositionNo = 290,
		FinancialStatus = 291,
		CorporateAction = 292,
		DefBidSize = 293,
		DefOfferSize = 294,
		NoQuoteEntries = 295,
		NoQuoteSets = 296,
		QuoteStatus = 297,
		QuoteCancelType = 298,
		QuoteEntryID = 299,
		QuoteRejectReason = 300,
		QuoteResponseLevel = 301,
		QuoteSetID = 302,
		QuoteRequestType = 303,
		TotNoQuoteEntries = 304,
		UnderlyingSecurityIDSource = 305,
		UnderlyingIssuer = 306,
		UnderlyingSecurityDesc = 307,
		UnderlyingSecurityExchange = 308,
		UnderlyingSecurityID = 309,
		UnderlyingSecurityType = 310,
		UnderlyingSymbol = 311,
		UnderlyingSymbolSfx = 312,
		UnderlyingMaturityMonthYear = 313,
		UnderlyingPutOrCall = 315,
		UnderlyingStrikePrice = 316,
		UnderlyingOptAttribute = 317,
		UnderlyingCurrency = 318,
		SecurityReqID = 320,
		SecurityRequestType = 321,
		SecurityResponseID = 322,
		SecurityResponseType = 323,
		SecurityStatusReqID = 324,
		UnsolicitedIndicator = 325,
		SecurityTradingStatus = 326,
		HaltReason = 327,
		InViewOfCommon = 328,
		DueToRelated = 329,
		BuyVolume = 330,
		SellVolume = 331,
		HighPx = 332,
		LowPx = 333,
		Adjustment = 334,
		TradSesReqID = 335,
		TradingSessionID = 336,
		ContraTrader = 337,
		TradSesMethod = 338,
		TradSesMode = 339,
		TradSesStatus = 340,
		TradSesStartTime = 341,
		TradSesOpenTime = 342,
		TradSesPreCloseTime = 343,
		TradSesCloseTime = 344,
		TradSesEndTime = 345,
		NumberOfOrders = 346,
		MessageEncoding = 347,
		EncodedIssuerLen = 348,
		EncodedIssuer = 349,
		EncodedSecurityDescLen = 350,
		EncodedSecurityDesc = 351,
		EncodedListExecInstLen = 352,
		EncodedListExecInst = 353,
		EncodedTextLen = 354,
		EncodedText = 355,
		EncodedSubjectLen = 356,
		EncodedSubject = 357,
		EncodedHeadlineLen = 358,
		EncodedHeadline = 359,
		EncodedAllocTextLen = 360,
		EncodedAllocText = 361,
		EncodedUnderlyingIssuerLen = 362,
		EncodedUnderlyingIssuer = 363,
		EncodedUnderlyingSecurityDescLen = 364,
		EncodedUnderlyingSecurityDesc = 365,
		AllocPrice = 366,
		QuoteSetValidUntilTime = 367,
		QuoteEntryRejectReason = 368,
		LastMsgSeqNumProcessed = 369,
		RefTagID = 371,
		RefMsgType = 372,
		SessionRejectReason = 373,
		BidRequestTransType = 374,
		ContraBroker = 375,
		ComplianceID = 376,
		SolicitedFlag = 377,
		ExecRestatementReason = 378,
		BusinessRejectRefID = 379,
		BusinessRejectReason = 380,
		GrossTradeAmt = 381,
		NoContraBrokers = 382,
		MaxMessageSize = 383,
		NoMsgTypes = 384,
		MsgDirection = 385,
		NoTradingSessions = 386,
		TotalVolumeTraded = 387,
		DiscretionInst = 388,
		DiscretionOffsetValue = 389,
		BidID = 390,
		ClientBidID = 391,
		ListName = 392,
		TotNoRelatedSym = 393,
		BidType = 394,
		NumTickets = 395,
		SideValue1 = 396,
		SideValue2 = 397,
		NoBidDescriptors = 398,
		BidDescriptorType = 399,
		BidDescriptor = 400,
		SideValueInd = 401,
		LiquidityPctLow = 402,
		LiquidityPctHigh = 403,
		LiquidityValue = 404,
		EFPTrackingError = 405,
		FairValue = 406,
		OutsideIndexPct = 407,
		ValueOfFutures = 408,
		LiquidityIndType = 409,
		WtAverageLiquidity = 410,
		ExchangeForPhysical = 411,
		OutMainCntryUIndex = 412,
		CrossPercent = 413,
		ProgRptReqs = 414,
		ProgPeriodInterval = 415,
		IncTaxInd = 416,
		NumBidders = 417,
		BidTradeType = 418,
		BasisPxType = 419,
		NoBidComponents = 420,
		Country = 421,
		TotNoStrikes = 422,
		PriceType = 423,
		DayOrderQty = 424,
		DayCumQty = 425,
		DayAvgPx = 426,
		GTBookingInst = 427,
		NoStrikes = 428,
		ListStatusType = 429,
		NetGrossInd = 430,
		ListOrderStatus = 431,
		ExpireDate = 432,
		ListExecInstType = 433,
		CxlRejResponseTo = 434,
		UnderlyingCouponRate = 435,
		UnderlyingContractMultiplier = 436,
		ContraTradeQty = 437,
		ContraTradeTime = 438,
		LiquidityNumSecurities = 441,
		MultiLegReportingType = 442,
		StrikeTime = 443,
		ListStatusText = 444,
		EncodedListStatusTextLen = 445,
		EncodedListStatusText = 446,
		PartyIDSource = 447,
		PartyID = 448,
		NetChgPrevDay = 451,
		PartyRole = 452,
		NoPartyIDs = 453,
		NoSecurityAltID = 454,
		SecurityAltID = 455,
		SecurityAltIDSource = 456,
		NoUnderlyingSecurityAltID = 457,
		UnderlyingSecurityAltID = 458,
		UnderlyingSecurityAltIDSource = 459,
		Product = 460,
		CFICode = 461,
		UnderlyingProduct = 462,
		UnderlyingCFICode = 463,
		TestMessageIndicator = 464,
		QuantityType = 465,
		BookingRefID = 466,
		IndividualAllocID = 467,
		RoundingDirection = 468,
		RoundingModulus = 469,
		CountryOfIssue = 470,
		StateOrProvinceOfIssue = 471,
		LocaleOfIssue = 472,
		NoRegistDtls = 473,
		MailingDtls = 474,
		InvestorCountryOfResidence = 475,
		PaymentRef = 476,
		DistribPaymentMethod = 477,
		CashDistribCurr = 478,
		CommCurrency = 479,
		CancellationRights = 480,
		MoneyLaunderingStatus = 481,
		MailingInst = 482,
		TransBkdTime = 483,
		ExecPriceType = 484,
		ExecPriceAdjustment = 485,
		DateOfBirth = 486,
		TradeReportTransType = 487,
		CardHolderName = 488,
		CardNumber = 489,
		CardExpDate = 490,
		CardIssNum = 491,
		PaymentMethod = 492,
		RegistAcctType = 493,
		Designation = 494,
		TaxAdvantageType = 495,
		RegistRejReasonText = 496,
		FundRenewWaiv = 497,
		CashDistribAgentName = 498,
		CashDistribAgentCode = 499,
		CashDistribAgentAcctNumber = 500,
		CashDistribPayRef = 501,
		CashDistribAgentAcctName = 502,
		CardStartDate = 503,
		PaymentDate = 504,
		PaymentRemitterID = 505,
		RegistStatus = 506,
		RegistRejReasonCode = 507,
		RegistRefID = 508,
		RegistDtls = 509,
		NoDistribInsts = 510,
		RegistEmail = 511,
		DistribPercentage = 512,
		RegistID = 513,
		RegistTransType = 514,
		ExecValuationPoint = 515,
		OrderPercent = 516,
		OwnershipType = 517,
		NoContAmts = 518,
		ContAmtType = 519,
		ContAmtValue = 520,
		ContAmtCurr = 521,
		OwnerType = 522,
		PartySubID = 523,
		NestedPartyID = 524,
		NestedPartyIDSource = 525,
		SecondaryClOrdID = 526,
		SecondaryExecID = 527,
		OrderCapacity = 528,
		OrderRestrictions = 529,
		MassCancelRequestType = 530,
		MassCancelResponse = 531,
		MassCancelRejectReason = 532,
		TotalAffectedOrders = 533,
		NoAffectedOrders = 534,
		AffectedOrderID = 535,
		AffectedSecondaryOrderID = 536,
		QuoteType = 537,
		NestedPartyRole = 538,
		NoNestedPartyIDs = 539,
		TotalAccruedInterestAmt = 540,
		MaturityDate = 541,
		UnderlyingMaturityDate = 542,
		InstrRegistry = 543,
		CashMargin = 544,
		NestedPartySubID = 545,
		Scope = 546,
		MDImplicitDelete = 547,
		CrossID = 548,
		CrossType = 549,
		CrossPrioritization = 550,
		OrigCrossID = 551,
		NoSides = 552,
		Username = 553,
		Password = 554,
		NoLegs = 555,
		LegCurrency = 556,
		TotNoSecurityTypes = 557,
		NoSecurityTypes = 558,
		SecurityListRequestType = 559,
		SecurityRequestResult = 560,
		RoundLot = 561,
		MinTradeVol = 562,
		MultiLegRptTypeReq = 563,
		LegPositionEffect = 564,
		LegCoveredOrUncovered = 565,
		LegPrice = 566,
		TradSesStatusRejReason = 567,
		TradeRequestID = 568,
		TradeRequestType = 569,
		PreviouslyReported = 570,
		TradeReportID = 571,
		TradeReportRefID = 572,
		MatchStatus = 573,
		MatchType = 574,
		OddLot = 575,
		NoClearingInstructions = 576,
		ClearingInstruction = 577,
		TradeInputSource = 578,
		TradeInputDevice = 579,
		NoDates = 580,
		AccountType = 581,
		CustOrderCapacity = 582,
		ClOrdLinkID = 583,
		MassStatusReqID = 584,
		MassStatusReqType = 585,
		OrigOrdModTime = 586,
		LegSettlType = 587,
		LegSettlDate = 588,
		DayBookingInst = 589,
		BookingUnit = 590,
		PreallocMethod = 591,
		UnderlyingCountryOfIssue = 592,
		UnderlyingStateOrProvinceOfIssue = 593,
		UnderlyingLocaleOfIssue = 594,
		UnderlyingInstrRegistry = 595,
		LegCountryOfIssue = 596,
		LegStateOrProvinceOfIssue = 597,
		LegLocaleOfIssue = 598,
		LegInstrRegistry = 599,
		LegSymbol = 600,
		LegSymbolSfx = 601,
		LegSecurityID = 602,
		LegSecurityIDSource = 603,
		NoLegSecurityAltID = 604,
		LegSecurityAltID = 605,
		LegSecurityAltIDSource = 606,
		LegProduct = 607,
		LegCFICode = 608,
		LegSecurityType = 609,
		LegMaturityMonthYear = 610,
		LegMaturityDate = 611,
		LegStrikePrice = 612,
		LegOptAttribute = 613,
		LegContractMultiplier = 614,
		LegCouponRate = 615,
		LegSecurityExchange = 616,
		LegIssuer = 617,
		EncodedLegIssuerLen = 618,
		EncodedLegIssuer = 619,
		LegSecurityDesc = 620,
		EncodedLegSecurityDescLen = 621,
		EncodedLegSecurityDesc = 622,
		LegRatioQty = 623,
		LegSide = 624,
		TradingSessionSubID = 625,
		AllocType = 626,
		NoHops = 627,
		HopCompID = 628,
		HopSendingTime = 629,
		HopRefID = 630,
		MidPx = 631,
		BidYield = 632,
		MidYield = 633,
		OfferYield = 634,
		ClearingFeeIndicator = 635,
		WorkingIndicator = 636,
		LegLastPx = 637,
		PriorityIndicator = 638,
		PriceImprovement = 639,
		Price2 = 640,
		LastForwardPoints2 = 641,
		BidForwardPoints2 = 642,
		OfferForwardPoints2 = 643,
		RFQReqID = 644,
		MktBidPx = 645,
		MktOfferPx = 646,
		MinBidSize = 647,
		MinOfferSize = 648,
		QuoteStatusReqID = 649,
		LegalConfirm = 650,
		UnderlyingLastPx = 651,
		UnderlyingLastQty = 652,
		LegRefID = 654,
		ContraLegRefID = 655,
		SettlCurrBidFxRate = 656,
		SettlCurrOfferFxRate = 657,
		QuoteRequestRejectReason = 658,
		SideComplianceID = 659,
		AcctIDSource = 660,
		AllocAcctIDSource = 661,
		BenchmarkPrice = 662,
		BenchmarkPriceType = 663,
		ConfirmID = 664,
		ConfirmStatus = 665,
		ConfirmTransType = 666,
		ContractSettlMonth = 667,
		DeliveryForm = 668,
		LastParPx = 669,
		NoLegAllocs = 670,
		LegAllocAccount = 671,
		LegIndividualAllocID = 672,
		LegAllocQty = 673,
		LegAllocAcctIDSource = 674,
		LegSettlCurrency = 675,
		LegBenchmarkCurveCurrency = 676,
		LegBenchmarkCurveName = 677,
		LegBenchmarkCurvePoint = 678,
		LegBenchmarkPrice = 679,
		LegBenchmarkPriceType = 680,
		LegBidPx = 681,
		LegIOIQty = 682,
		NoLegStipulations = 683,
		LegOfferPx = 684,
		LegOrderQty = 685,
		LegPriceType = 686,
		LegQty = 687,
		LegStipulationType = 688,
		LegStipulationValue = 689,
		LegSwapType = 690,
		Pool = 691,
		QuotePriceType = 692,
		QuoteRespID = 693,
		QuoteRespType = 694,
		QuoteQualifier = 695,
		YieldRedemptionDate = 696,
		YieldRedemptionPrice = 697,
		YieldRedemptionPriceType = 698,
		BenchmarkSecurityID = 699,
		ReversalIndicator = 700,
		YieldCalcDate = 701,
		NoPositions = 702,
		PosType = 703,
		LongQty = 704,
		ShortQty = 705,
		PosQtyStatus = 706,
		PosAmtType = 707,
		PosAmt = 708,
		PosTransType = 709,
		PosReqID = 710,
		NoUnderlyings = 711,
		PosMaintAction = 712,
		OrigPosReqRefID = 713,
		PosMaintRptRefID = 714,
		ClearingBusinessDate = 715,
		SettlSessID = 716,
		SettlSessSubID = 717,
		AdjustmentType = 718,
		ContraryInstructionIndicator = 719,
		PriorSpreadIndicator = 720,
		PosMaintRptID = 721,
		PosMaintStatus = 722,
		PosMaintResult = 723,
		PosReqType = 724,
		ResponseTransportType = 725,
		ResponseDestination = 726,
		TotalNumPosReports = 727,
		PosReqResult = 728,
		PosReqStatus = 729,
		SettlPrice = 730,
		SettlPriceType = 731,
		UnderlyingSettlPrice = 732,
		UnderlyingSettlPriceType = 733,
		PriorSettlPrice = 734,
		NoQuoteQualifiers = 735,
		AllocSettlCurrency = 736,
		AllocSettlCurrAmt = 737,
		InterestAtMaturity = 738,
		LegDatedDate = 739,
		LegPool = 740,
		AllocInterestAtMaturity = 741,
		AllocAccruedInterestAmt = 742,
		DeliveryDate = 743,
		AssignmentMethod = 744,
		AssignmentUnit = 745,
		OpenInterest = 746,
		ExerciseMethod = 747,
		TotNumTradeReports = 748,
		TradeRequestResult = 749,
		TradeRequestStatus = 750,
		TradeReportRejectReason = 751,
		SideMultiLegReportingType = 752,
		NoPosAmt = 753,
		AutoAcceptIndicator = 754,
		AllocReportID = 755,
		NoNested2PartyIDs = 756,
		Nested2PartyID = 757,
		Nested2PartyIDSource = 758,
		Nested2PartyRole = 759,
		Nested2PartySubID = 760,
		BenchmarkSecurityIDSource = 761,
		SecuritySubType = 762,
		UnderlyingSecuritySubType = 763,
		LegSecuritySubType = 764,
		AllowableOneSidednessPct = 765,
		AllowableOneSidednessValue = 766,
		AllowableOneSidednessCurr = 767,
		NoTrdRegTimestamps = 768,
		TrdRegTimestamp = 769,
		TrdRegTimestampType = 770,
		TrdRegTimestampOrigin = 771,
		ConfirmRefID = 772,
		ConfirmType = 773,
		ConfirmRejReason = 774,
		BookingType = 775,
		IndividualAllocRejCode = 776,
		SettlInstMsgID = 777,
		NoSettlInst = 778,
		LastUpdateTime = 779,
		AllocSettlInstType = 780,
		NoSettlPartyIDs = 781,
		SettlPartyID = 782,
		SettlPartyIDSource = 783,
		SettlPartyRole = 784,
		SettlPartySubID = 785,
		SettlPartySubIDType = 786,
		DlvyInstType = 787,
		TerminationType = 788,
		NextExpectedMsgSeqNum = 789,
		OrdStatusReqID = 790,
		SettlInstReqID = 791,
		SettlInstReqRejCode = 792,
		SecondaryAllocID = 793,
		AllocReportType = 794,
		AllocReportRefID = 795,
		AllocCancReplaceReason = 796,
		CopyMsgIndicator = 797,
		AllocAccountType = 798,
		OrderAvgPx = 799,
		OrderBookingQty = 800,
		NoSettlPartySubIDs = 801,
		NoPartySubIDs = 802,
		PartySubIDType = 803,
		NoNestedPartySubIDs = 804,
		NestedPartySubIDType = 805,
		NoNested2PartySubIDs = 806,
		Nested2PartySubIDType = 807,
		AllocIntermedReqType = 808,
		UnderlyingPx = 810,
		PriceDelta = 811,
		ApplQueueMax = 812,
		ApplQueueDepth = 813,
		ApplQueueResolution = 814,
		ApplQueueAction = 815,
		NoAltMDSource = 816,
		AltMDSourceID = 817,
		SecondaryTradeReportID = 818,
		AvgPxIndicator = 819,
		TradeLinkID = 820,
		OrderInputDevice = 821,
		UnderlyingTradingSessionID = 822,
		UnderlyingTradingSessionSubID = 823,
		TradeLegRefID = 824,
		ExchangeRule = 825,
		TradeAllocIndicator = 826,
		ExpirationCycle = 827,
		TrdType = 828,
		TrdSubType = 829,
		TransferReason = 830,
		AsgnReqID = 831,
		TotNumAssignmentReports = 832,
		AsgnRptID = 833,
		ThresholdAmount = 834,
		PegMoveType = 835,
		PegOffsetType = 836,
		PegLimitType = 837,
		PegRoundDirection = 838,
		PeggedPrice = 839,
		PegScope = 840,
		DiscretionMoveType = 841,
		DiscretionOffsetType = 842,
		DiscretionLimitType = 843,
		DiscretionRoundDirection = 844,
		DiscretionPrice = 845,
		DiscretionScope = 846,
		TargetStrategy = 847,
		TargetStrategyParameters = 848,
		ParticipationRate = 849,
		TargetStrategyPerformance = 850,
		LastLiquidityInd = 851,
		PublishTrdIndicator = 852,
		ShortSaleReason = 853,
		QtyType = 854,
		SecondaryTrdType = 855,
		TradeReportType = 856,
		AllocNoOrdersType = 857,
		SharedCommission = 858,
		ConfirmReqID = 859,
		AvgParPx = 860,
		ReportedPx = 861,
		NoCapacities = 862,
		OrderCapacityQty = 863,
		NoEvents = 864,
		EventType = 865,
		EventDate = 866,
		EventPx = 867,
		EventText = 868,
		PctAtRisk = 869,
		NoInstrAttrib = 870,
		InstrAttribType = 871,
		InstrAttribValue = 872,
		DatedDate = 873,
		InterestAccrualDate = 874,
		CPProgram = 875,
		CPRegType = 876,
		UnderlyingCPProgram = 877,
		UnderlyingCPRegType = 878,
		UnderlyingQty = 879,
		TrdMatchID = 880,
		SecondaryTradeReportRefID = 881,
		UnderlyingDirtyPrice = 882,
		UnderlyingEndPrice = 883,
		UnderlyingStartValue = 884,
		UnderlyingCurrentValue = 885,
		UnderlyingEndValue = 886,
		NoUnderlyingStips = 887,
		UnderlyingStipType = 888,
		UnderlyingStipValue = 889,
		MaturityNetMoney = 890,
		MiscFeeBasis = 891,
		TotNoAllocs = 892,
		LastFragment = 893,
		CollReqID = 894,
		CollAsgnReason = 895,
		CollInquiryQualifier = 896,
		NoTrades = 897,
		MarginRatio = 898,
		MarginExcess = 899,
		TotalNetValue = 900,
		CashOutstanding = 901,
		CollAsgnID = 902,
		CollAsgnTransType = 903,
		CollRespID = 904,
		CollAsgnRespType = 905,
		CollAsgnRejectReason = 906,
		CollAsgnRefID = 907,
		CollRptID = 908,
		CollInquiryID = 909,
		CollStatus = 910,
		TotNumReports = 911,
		LastRptRequested = 912,
		AgreementDesc = 913,
		AgreementID = 914,
		AgreementDate = 915,
		StartDate = 916,
		EndDate = 917,
		AgreementCurrency = 918,
		DeliveryType = 919,
		EndAccruedInterestAmt = 920,
		StartCash = 921,
		EndCash = 922,
		UserRequestID = 923,
		UserRequestType = 924,
		NewPassword = 925,
		UserStatus = 926,
		UserStatusText = 927,
		StatusValue = 928,
		StatusText = 929,
		RefCompID = 930,
		RefSubID = 931,
		NetworkResponseID = 932,
		NetworkRequestID = 933,
		LastNetworkResponseID = 934,
		NetworkRequestType = 935,
		NoCompIDs = 936,
		NetworkStatusResponseType = 937,
		NoCollInquiryQualifier = 938,
		TrdRptStatus = 939,
		AffirmStatus = 940,
		UnderlyingStrikeCurrency = 941,
		LegStrikeCurrency = 942,
		TimeBracket = 943,
		CollAction = 944,
		CollInquiryStatus = 945,
		CollInquiryResult = 946,
		StrikeCurrency = 947,
		NoNested3PartyIDs = 948,
		Nested3PartyID = 949,
		Nested3PartyIDSource = 950,
		Nested3PartyRole = 951,
		NoNested3PartySubIDs = 952,
		Nested3PartySubID = 953,
		Nested3PartySubIDType = 954,
		LegContractSettlMonth = 955,
		LegInterestAccrualDate = 956,
		CancelOnDisconnect = 9001,
		MDEntryMakerSide = 9002,
		EnableMDEntryMakerSide = 9003,
		MAX_VAL = 9004
	};

	namespace Account_val {
	};

	namespace AdvId_val {
	};

	namespace AdvRefID_val {
	};

	namespace AdvSide_val {
		static const char BUY[] = "B";
		static const char SELL[] = "S";
		static const char CROSS[] = "X";
		static const char TRADE[] = "T";
	};

	namespace AdvTransType_val {
		static const char NEW[] = "N";
		static const char CANCEL[] = "C";
		static const char REPLACE[] = "R";
	};

	namespace BeginString_val {
	};

	namespace CheckSum_val {
	};

	namespace ClOrdID_val {
	};

	namespace CommType_val {
		static const char PER_UNIT[] = "1";
		static const char PERCENTAGE[] = "2";
		static const char ABSOLUTE[] = "3";
		static const char PERCENTAGE_WAIVED_CASH_DISCOUNT[] = "4";
		static const char PERCENTAGE_WAIVED_ENHANCED_UNITS[] = "5";
		static const char POINTS_PER_BOND_OR_OR_CONTRACT[] = "6";
	};

	namespace ExecID_val {
	};

	namespace ExecRefID_val {
	};

	namespace HandlInst_val {
		static const char AUTOMATED_EXECUTION_ORDER_PRIVATE[] = "1";
		static const char AUTOMATED_EXECUTION_ORDER_PUBLIC[] = "2";
		static const char MANUAL_ORDER[] = "3";
	};

	namespace SecurityIDSource_val {
		static const char CUSIP[] = "1";
		static const char SEDOL[] = "2";
		static const char QUIK[] = "3";
		static const char ISIN_NUMBER[] = "4";
		static const char RIC_CODE[] = "5";
		static const char ISO_CURRENCY_CODE[] = "6";
		static const char ISO_COUNTRY_CODE[] = "7";
		static const char EXCHANGE_SYMBOL[] = "8";
		static const char CONSOLIDATED_TAPE_ASSOCIATION[] = "9";
		static const char BLOOMBERG_SYMBOL[] = "A";
		static const char WERTPAPIER[] = "B";
		static const char DUTCH[] = "C";
		static const char VALOREN[] = "D";
		static const char SICOVAM[] = "E";
		static const char BELGIAN[] = "F";
		static const char COMMON[] = "G";
		static const char CLEARING_HOUSE_CLEARING_ORGANIZATION[] = "H";
		static const char ISDA_FPML_PRODUCT_SPECIFICATION[] = "I";
		static const char OPTIONS_PRICE_REPORTING_AUTHORITY[] = "J";
	};

	namespace IOIID_val {
	};

	namespace IOIQltyInd_val {
		static const char LOW[] = "L";
		static const char MEDIUM[] = "M";
		static const char HIGH[] = "H";
	};

	namespace IOIRefID_val {
	};

	namespace IOIQty_val {
	};

	namespace IOITransType_val {
		static const char NEW[] = "N";
		static const char CANCEL[] = "C";
		static const char REPLACE[] = "R";
	};

	namespace LastCapacity_val {
		static const char AGENT[] = "1";
		static const char CROSS_AS_AGENT[] = "2";
		static const char CROSS_AS_PRINCIPAL[] = "3";
		static const char PRINCIPAL[] = "4";
	};

	namespace MsgType_val {
		static const char HEARTBEAT[] = "0";
		static const char TEST_REQUEST[] = "1";
		static const char RESEND_REQUEST[] = "2";
		static const char REJECT[] = "3";
		static const char SEQUENCE_RESET[] = "4";
		static const char LOGOUT[] = "5";
		static const char INDICATION_OF_INTEREST[] = "6";
		static const char ADVERTISEMENT[] = "7";
		static const char EXECUTION_REPORT[] = "8";
		static const char ORDER_CANCEL_REJECT[] = "9";
		static const char LOGON[] = "A";
		static const char NEWS[] = "B";
		static const char EMAIL[] = "C";
		static const char ORDER_SINGLE[] = "D";
		static const char ORDER_LIST[] = "E";
		static const char ORDER_CANCEL_REQUEST[] = "F";
		static const char ORDER_CANCEL_REPLACE_REQUEST[] = "G";
		static const char ORDER_STATUS_REQUEST[] = "H";
		static const char ALLOCATION_INSTRUCTION[] = "J";
		static const char LIST_CANCEL_REQUEST[] = "K";
		static const char LIST_EXECUTE[] = "L";
		static const char LIST_STATUS_REQUEST[] = "M";
		static const char LIST_STATUS[] = "N";
		static const char ALLOCATION_INSTRUCTION_ACK[] = "P";
		static const char DONT_KNOW_TRADE[] = "Q";
		static const char QUOTE_REQUEST[] = "R";
		static const char QUOTE[] = "S";
		static const char SETTLEMENT_INSTRUCTIONS[] = "T";
		static const char MARKET_DATA_REQUEST[] = "V";
		static const char MARKET_DATA_SNAPSHOT_FULL_REFRESH[] = "W";
		static const char MARKET_DATA_INCREMENTAL_REFRESH[] = "X";
		static const char MARKET_DATA_REQUEST_REJECT[] = "Y";
		static const char QUOTE_CANCEL[] = "Z";
		static const char QUOTE_STATUS_REQUEST[] = "a";
		static const char MASS_QUOTE_ACKNOWLEDGEMENT[] = "b";
		static const char SECURITY_DEFINITION_REQUEST[] = "c";
		static const char SECURITY_DEFINITION[] = "d";
		static const char SECURITY_STATUS_REQUEST[] = "e";
		static const char SECURITY_STATUS[] = "f";
		static const char TRADING_SESSION_STATUS_REQUEST[] = "g";
		static const char TRADING_SESSION_STATUS[] = "h";
		static const char MASS_QUOTE[] = "i";
		static const char BUSINESS_MESSAGE_REJECT[] = "j";
		static const char BID_REQUEST[] = "k";
		static const char BID_RESPONSE[] = "l";
		static const char LIST_STRIKE_PRICE[] = "m";
		static const char XML_MESSAGE[] = "n";
		static const char REGISTRATION_INSTRUCTIONS[] = "o";
		static const char REGISTRATION_INSTRUCTIONS_RESPONSE[] = "p";
		static const char ORDER_MASS_CANCEL_REQUEST[] = "q";
		static const char ORDER_MASS_CANCEL_REPORT[] = "r";
		static const char NEW_ORDER_CROSS[] = "s";
		static const char CROSS_ORDER_CANCEL_REPLACE_REQUEST[] = "t";
		static const char CROSS_ORDER_CANCEL_REQUEST[] = "u";
		static const char SECURITY_TYPE_REQUEST[] = "v";
		static const char SECURITY_TYPES[] = "w";
		static const char SECURITY_LIST_REQUEST[] = "x";
		static const char SECURITY_LIST[] = "y";
		static const char DERIVATIVE_SECURITY_LIST_REQUEST[] = "z";
		static const char DERIVATIVE_SECURITY_LIST[] = "AA";
		static const char NEW_ORDER_MULTILEG[] = "AB";
		static const char MULTILEG_ORDER_CANCEL_REPLACE[] = "AC";
		static const char TRADE_CAPTURE_REPORT_REQUEST[] = "AD";
		static const char TRADE_CAPTURE_REPORT[] = "AE";
		static const char ORDER_MASS_STATUS_REQUEST[] = "AF";
		static const char QUOTE_REQUEST_REJECT[] = "AG";
		static const char RFQ_REQUEST[] = "AH";
		static const char QUOTE_STATUS_REPORT[] = "AI";
		static const char QUOTE_RESPONSE[] = "AJ";
		static const char CONFIRMATION[] = "AK";
		static const char POSITION_MAINTENANCE_REQUEST[] = "AL";
		static const char POSITION_MAINTENANCE_REPORT[] = "AM";
		static const char REQUEST_FOR_POSITIONS[] = "AN";
		static const char REQUEST_FOR_POSITIONS_ACK[] = "AO";
		static const char POSITION_REPORT[] = "AP";
		static const char TRADE_CAPTURE_REPORT_REQUEST_ACK[] = "AQ";
		static const char TRADE_CAPTURE_REPORT_ACK[] = "AR";
		static const char ALLOCATION_REPORT[] = "AS";
		static const char ALLOCATION_REPORT_ACK[] = "AT";
		static const char CONFIRMATION_ACK[] = "AU";
		static const char SETTLEMENT_INSTRUCTION_REQUEST[] = "AV";
		static const char ASSIGNMENT_REPORT[] = "AW";
		static const char COLLATERAL_REQUEST[] = "AX";
		static const char COLLATERAL_ASSIGNMENT[] = "AY";
		static const char COLLATERAL_RESPONSE[] = "AZ";
		static const char COLLATERAL_REPORT[] = "BA";
		static const char COLLATERAL_INQUIRY[] = "BB";
		static const char NETWORK_STATUS_REQUEST[] = "BC";
		static const char NETWORK_STATUS_RESPONSE[] = "BD";
		static const char USER_REQUEST[] = "BE";
		static const char USER_RESPONSE[] = "BF";
		static const char COLLATERAL_INQUIRY_ACK[] = "BG";
		static const char CONFIRMATION_REQUEST[] = "BH";
	};

	namespace OrderID_val {
	};

	namespace OrdStatus_val {
		static const char NEW[] = "0";
		static const char PARTIALLY_FILLED[] = "1";
		static const char FILLED[] = "2";
		static const char DONE_FOR_DAY[] = "3";
		static const char CANCELED[] = "4";
		static const char REPLACED[] = "5";
		static const char PENDING_CANCEL[] = "6";
		static const char STOPPED[] = "7";
		static const char REJECTED[] = "8";
		static const char SUSPENDED[] = "9";
		static const char PENDING_NEW[] = "A";
		static const char CALCULATED[] = "B";
		static const char EXPIRED[] = "C";
		static const char ACCEPTED_FOR_BIDDING[] = "D";
		static const char PENDING_REPLACE[] = "E";
	};

	namespace OrdType_val {
		static const char MARKET[] = "1";
		static const char LIMIT[] = "2";
		static const char STOP[] = "3";
		static const char STOP_LIMIT[] = "4";
		static const char MARKET_ON_CLOSE[] = "5";
		static const char WITH_OR_WITHOUT[] = "6";
		static const char LIMIT_OR_BETTER[] = "7";
		static const char LIMIT_WITH_OR_WITHOUT[] = "8";
		static const char ON_BASIS[] = "9";
		static const char ON_CLOSE[] = "A";
		static const char LIMIT_ON_CLOSE[] = "B";
		static const char FOREX_MARKET[] = "C";
		static const char PREVIOUSLY_QUOTED[] = "D";
		static const char PREVIOUSLY_INDICATED[] = "E";
		static const char FOREX_LIMIT[] = "F";
		static const char FOREX_SWAP[] = "G";
		static const char FOREX_PREVIOUSLY_QUOTED[] = "H";
		static const char FUNARI[] = "I";
		static const char MARKET_IF_TOUCHED[] = "J";
		static const char MARKET_WITH_LEFTOVER_AS_LIMIT[] = "K";
		static const char PREVIOUS_FUND_VALUATION_POINT[] = "L";
		static const char NEXT_FUND_VALUATION_POINT[] = "M";
		static const char PEGGED[] = "P";
	};

	namespace OrigClOrdID_val {
	};

	namespace SecurityID_val {
	};

	namespace SenderCompID_val {
	};

	namespace SenderSubID_val {
	};

	namespace Side_val {
		static const char BUY[] = "1";
		static const char SELL[] = "2";
		static const char BUY_MINUS[] = "3";
		static const char SELL_PLUS[] = "4";
		static const char SELL_SHORT[] = "5";
		static const char SELL_SHORT_EXEMPT[] = "6";
		static const char UNDISCLOSED[] = "7";
		static const char CROSS[] = "8";
		static const char CROSS_SHORT[] = "9";
		static const char CROSS_SHORT_EXEMPT[] = "A";
		static const char AS_DEFINED[] = "B";
		static const char OPPOSITE[] = "C";
		static const char SUBSCRIBE[] = "D";
		static const char REDEEM[] = "E";
		static const char LEND[] = "F";
		static const char BORROW[] = "G";
	};

	namespace Symbol_val {
	};

	namespace TargetCompID_val {
	};

	namespace TargetSubID_val {
	};

	namespace Text_val {
	};

	namespace TimeInForce_val {
		static const char DAY[] = "0";
		static const char GOOD_TILL_CANCEL[] = "1";
		static const char AT_THE_OPENING[] = "2";
		static const char IMMEDIATE_OR_CANCEL[] = "3";
		static const char FILL_OR_KILL[] = "4";
		static const char GOOD_TILL_CROSSING[] = "5";
		static const char GOOD_TILL_DATE[] = "6";
		static const char AT_THE_CLOSE[] = "7";
	};

	namespace Urgency_val {
		static const char NORMAL[] = "0";
		static const char FLASH[] = "1";
		static const char BACKGROUND[] = "2";
	};

	namespace SettlType_val {
		static const char REGULAR[] = "0";
		static const char CASH[] = "1";
		static const char NEXT_DAY[] = "2";
		static const char T_PLUS_2[] = "3";
		static const char T_PLUS_3[] = "4";
		static const char T_PLUS_4[] = "5";
		static const char FUTURE[] = "6";
		static const char WHEN_AND_IF_ISSUED[] = "7";
		static const char SELLERS_OPTION[] = "8";
		static const char T_PLUS_5[] = "9";
	};

	namespace SymbolSfx_val {
		static const char WHEN_ISSUED[] = "WI";
		static const char A_EUCP_WITH_LUMP_SUM_INTEREST[] = "CD";
	};

	namespace ListID_val {
	};

	namespace ListSeqNo_val {
	};

	namespace TotNoOrders_val {
	};

	namespace ListExecInst_val {
	};

	namespace AllocID_val {
	};

	namespace AllocTransType_val {
		static const char NEW[] = "0";
		static const char REPLACE[] = "1";
		static const char CANCEL[] = "2";
		static const char PRELIMINARY[] = "3";
		static const char CALCULATED[] = "4";
		static const char CALCULATED_WITHOUT_PRELIMINARY[] = "5";
	};

	namespace RefAllocID_val {
	};

	namespace AvgPxPrecision_val {
	};

	namespace PositionEffect_val {
		static const char OPEN[] = "O";
		static const char CLOSE[] = "C";
		static const char ROLLED[] = "R";
		static const char FIFO[] = "F";
	};

	namespace AllocAccount_val {
	};

	namespace ProcessCode_val {
		static const char REGULAR[] = "0";
		static const char SOFT_DOLLAR[] = "1";
		static const char STEP_IN[] = "2";
		static const char STEP_OUT[] = "3";
		static const char SOFT_DOLLAR_STEP_IN[] = "4";
		static const char SOFT_DOLLAR_STEP_OUT[] = "5";
		static const char PLAN_SPONSOR[] = "6";
	};

	namespace RptSeq_val {
	};

	namespace AllocStatus_val {
		static int ACCEPTED = 0;
		static int BLOCK_LEVEL_REJECT = 1;
		static int ACCOUNT_LEVEL_REJECT = 2;
		static int RECEIVED = 3;
		static int INCOMPLETE = 4;
		static int REJECTED_BY_INTERMEDIARY = 5;
	};

	namespace AllocRejCode_val {
		static int UNKNOWN_ACCOUNT = 0;
		static int INCORRECT_QUANTITY = 1;
		static int INCORRECT_AVERAGE_PRICE = 2;
		static int UNKNOWN_EXECUTING_BROKER_MNEMONIC = 3;
		static int COMMISSION_DIFFERENCE = 4;
		static int UNKNOWN_ORDERID = 5;
		static int UNKNOWN_LISTID = 6;
		static int OTHER = 7;
		static int INCORRECT_ALLOCATED_QUANTITY = 8;
		static int CALCULATION_DIFFERENCE = 9;
		static int UNKNOWN_OR_STALE_EXEC_ID = 10;
		static int MISMATCHED_DATA_VALUE = 11;
		static int UNKNOWN_CLORDID = 12;
		static int WAREHOUSE_REQUEST_REJECTED = 13;
	};

	namespace EmailType_val {
		static const char NEW[] = "0";
		static const char REPLY[] = "1";
		static const char ADMIN_REPLY[] = "2";
	};

	namespace EncryptMethod_val {
		static int NONE_OTHER = 0;
		static int PKCS = 1;
		static int DES = 2;
		static int PKCS_DES = 3;
		static int PGP_DES = 4;
		static int PGP_DES_MD5 = 5;
		static int PEM_DES_MD5 = 6;
	};

	namespace CxlRejReason_val {
		static int TOO_LATE_TO_CANCEL = 0;
		static int UNKNOWN_ORDER = 1;
		static int BROKER_EXCHANGE_OPTION = 2;
		static int ORDER_ALREADY_IN_PENDING_CANCEL_OR_PENDING_REPLACE_STATUS = 3;
		static int UNABLE_TO_PROCESS_ORDER_MASS_CANCEL_REQUEST = 4;
		static int ORIGORDMODTIME_DID_NOT_MATCH_LAST_TRANSACTTIME_OF_ORDER = 5;
		static int DUPLICATE_CLORDID_RECEIVED = 6;
		static int OTHER = 99;
	};

	namespace OrdRejReason_val {
		static int BROKER_EXCHANGE_OPTION = 0;
		static int UNKNOWN_SYMBOL = 1;
		static int EXCHANGE_CLOSED = 2;
		static int ORDER_EXCEEDS_LIMIT = 3;
		static int TOO_LATE_TO_ENTER = 4;
		static int UNKNOWN_ORDER = 5;
		static int DUPLICATE_ORDER = 6;
		static int DUPLICATE_OF_A_VERBALLY_COMMUNICATED_ORDER = 7;
		static int STALE_ORDER = 8;
		static int TRADE_ALONG_REQUIRED = 9;
		static int INVALID_INVESTOR_ID = 10;
		static int UNSUPPORTED_ORDER_CHARACTERISTIC = 11;
		static int SURVEILLENCE_OPTION = 12;
		static int INCORRECT_QUANTITY = 13;
		static int INCORRECT_ALLOCATED_QUANTITY = 14;
		static int UNKNOWN_ACCOUNT = 15;
		static int OTHER = 99;
	};

	namespace IOIQualifier_val {
		static const char ALL_OR_NONE[] = "A";
		static const char MARKET_ON_CLOSE[] = "B";
		static const char AT_THE_CLOSE[] = "C";
		static const char VWAP[] = "D";
		static const char IN_TOUCH_WITH[] = "I";
		static const char LIMIT[] = "L";
		static const char MORE_BEHIND[] = "M";
		static const char AT_THE_OPEN[] = "O";
		static const char TAKING_A_POSITION[] = "P";
		static const char AT_THE_MARKET[] = "Q";
		static const char READY_TO_TRADE[] = "R";
		static const char PORTFOLIO_SHOWN[] = "S";
		static const char THROUGH_THE_DAY[] = "T";
		static const char VERSUS[] = "V";
		static const char INDICATION_WORKING_AWAY[] = "W";
		static const char CROSSING_OPPORTUNITY[] = "X";
		static const char AT_THE_MIDPOINT[] = "Y";
		static const char PRE_OPEN[] = "Z";
	};

	namespace WaveNo_val {
	};

	namespace Issuer_val {
	};

	namespace SecurityDesc_val {
	};

	namespace HeartBtInt_val {
	};

	namespace TestReqID_val {
	};

	namespace OnBehalfOfCompID_val {
	};

	namespace OnBehalfOfSubID_val {
	};

	namespace QuoteID_val {
	};

	namespace DKReason_val {
		static const char UNKNOWN_SYMBOL[] = "A";
		static const char WRONG_SIDE[] = "B";
		static const char QUANTITY_EXCEEDS_ORDER[] = "C";
		static const char NO_MATCHING_ORDER[] = "D";
		static const char PRICE_EXCEEDS_LIMIT[] = "E";
		static const char CALCULATION_DIFFERENCE[] = "F";
		static const char OTHER[] = "Z";
	};

	namespace DeliverToCompID_val {
	};

	namespace DeliverToSubID_val {
	};

	namespace QuoteReqID_val {
	};

	namespace MiscFeeType_val {
		static const char REGULATORY[] = "1";
		static const char TAX[] = "2";
		static const char LOCAL_COMMISSION[] = "3";
		static const char EXCHANGE_FEES[] = "4";
		static const char STAMP[] = "5";
		static const char LEVY[] = "6";
		static const char OTHER[] = "7";
		static const char MARKUP[] = "8";
		static const char CONSUMPTION_TAX[] = "9";
		static const char PER_TRANSACTION[] = "10";
		static const char CONVERSION[] = "11";
		static const char AGENT[] = "12";
	};

	namespace SenderLocationID_val {
	};

	namespace TargetLocationID_val {
	};

	namespace OnBehalfOfLocationID_val {
	};

	namespace DeliverToLocationID_val {
	};

	namespace Subject_val {
	};

	namespace Headline_val {
	};

	namespace URLLink_val {
	};

	namespace ExecType_val {
		static const char NEW[] = "0";
		static const char PARTIAL_FILL[] = "1";
		static const char FILL[] = "2";
		static const char DONE_FOR_DAY[] = "3";
		static const char CANCELED[] = "4";
		static const char REPLACE[] = "5";
		static const char PENDING_CANCEL[] = "6";
		static const char STOPPED[] = "7";
		static const char REJECTED[] = "8";
		static const char SUSPENDED[] = "9";
		static const char PENDING_NEW[] = "A";
		static const char CALCULATED[] = "B";
		static const char EXPIRED[] = "C";
		static const char RESTATED[] = "D";
		static const char PENDING_REPLACE[] = "E";
		static const char TRADE[] = "F";
		static const char TRADE_CORRECT[] = "G";
		static const char TRADE_CANCEL[] = "H";
		static const char ORDER_STATUS[] = "I";
	};

	namespace SettlCurrFxRateCalc_val {
		static const char MULTIPLY[] = "M";
		static const char DIVIDE[] = "D";
	};

	namespace NumDaysInterest_val {
	};

	namespace SettlInstMode_val {
		static const char DEFAULT[] = "0";
		static const char STANDING_INSTRUCTIONS_PROVIDED[] = "1";
		static const char SPECIFIC_ORDER_FOR_A_SINGLE_ACCOUNT[] = "4";
		static const char REQUEST_REJECT[] = "5";
	};

	namespace AllocText_val {
	};

	namespace SettlInstID_val {
	};

	namespace SettlInstTransType_val {
		static const char NEW[] = "N";
		static const char CANCEL[] = "C";
		static const char REPLACE[] = "R";
		static const char RESTATE[] = "T";
	};

	namespace EmailThreadID_val {
	};

	namespace SettlInstSource_val {
		static const char BROKERS_INSTRUCTIONS[] = "1";
		static const char INSTITUTIONS_INSTRUCTIONS[] = "2";
		static const char INVESTOR[] = "3";
	};

	namespace SecurityType_val {
		static const char WILDCARD[] = "?";
		static const char ASSET_BACKED_SECURITIES[] = "ABS";
		static const char AMENDED_AND_RESTATED[] = "AMENDED";
		static const char OTHER_ANTICIPATION_NOTES[] = "AN";
		static const char BANKERS_ACCEPTANCE[] = "BA";
		static const char BANK_NOTES[] = "BN";
		static const char BILL_OF_EXCHANGES[] = "BOX";
		static const char BRADY_BOND[] = "BRADY";
		static const char BRIDGE_LOAN[] = "BRIDGE";
		static const char BUY_SELLBACK[] = "BUYSELL";
		static const char CONVERTIBLE_BOND[] = "CB";
		static const char CERTIFICATE_OF_DEPOSIT[] = "CD";
		static const char CALL_LOANS[] = "CL";
		static const char CORP_MORTGAGE_BACKED_SECURITIES[] = "CMBS";
		static const char COLLATERALIZED_MORTGAGE_OBLIGATION[] = "CMO";
		static const char CERTIFICATE_OF_OBLIGATION[] = "COFO";
		static const char CERTIFICATE_OF_PARTICIPATION[] = "COFP";
		static const char CORPORATE_BOND[] = "CORP";
		static const char COMMERCIAL_PAPER[] = "CP";
		static const char CORPORATE_PRIVATE_PLACEMENT[] = "CPP";
		static const char COMMON_STOCK[] = "CS";
		static const char DEFAULTED[] = "DEFLTED";
		static const char DEBTOR_IN_POSSESSION[] = "DINP";
		static const char DEPOSIT_NOTES[] = "DN";
		static const char DUAL_CURRENCY[] = "DUAL";
		static const char EURO_CERTIFICATE_OF_DEPOSIT[] = "EUCD";
		static const char EURO_CORPORATE_BOND[] = "EUCORP";
		static const char EURO_COMMERCIAL_PAPER[] = "EUCP";
		static const char EURO_SOVEREIGNS[] = "EUSOV";
		static const char EURO_SUPRANATIONAL_COUPONS[] = "EUSUPRA";
		static const char FEDERAL_AGENCY_COUPON[] = "FAC";
		static const char FEDERAL_AGENCY_DISCOUNT_NOTE[] = "FADN";
		static const char FOREIGN_EXCHANGE_CONTRACT[] = "FOR";
		static const char FORWARD[] = "FORWARD";
		static const char FUTURE[] = "FUT";
		static const char GENERAL_OBLIGATION_BONDS[] = "GO";
		static const char IOETTE_MORTGAGE[] = "IET";
		static const char LETTER_OF_CREDIT[] = "LOFC";
		static const char LIQUIDITY_NOTE[] = "LQN";
		static const char MATURED[] = "MATURED";
		static const char MORTGAGE_BACKED_SECURITIES[] = "MBS";
		static const char MUTUAL_FUND[] = "MF";
		static const char MORTGAGE_INTEREST_ONLY[] = "MIO";
		static const char MULTI_LEG_INSTRUMENT[] = "MLEG";
		static const char MORTGAGE_PRINCIPAL_ONLY[] = "MPO";
		static const char MORTGAGE_PRIVATE_PLACEMENT[] = "MPP";
		static const char MISCELLANEOUS_PASS_THROUGH[] = "MPT";
		static const char MANDATORY_TENDER[] = "MT";
		static const char MEDIUM_TERM_NOTES[] = "MTN";
		static const char NO_SECURITY_TYPE[] = "NONE";
		static const char OVERNIGHT[] = "ONITE";
		static const char OPTION[] = "OPT";
		static const char PRIVATE_EXPORT_FUNDING[] = "PEF";
		static const char PFANDBRIEFE[] = "PFAND";
		static const char PROMISSORY_NOTE[] = "PN";
		static const char PREFERRED_STOCK[] = "PS";
		static const char PLAZOS_FIJOS[] = "PZFJ";
		static const char REVENUE_ANTICIPATION_NOTE[] = "RAN";
		static const char REPLACED[] = "REPLACD";
		static const char REPURCHASE[] = "REPO";
		static const char RETIRED[] = "RETIRED";
		static const char REVENUE_BONDS[] = "REV";
		static const char REVOLVER_LOAN[] = "RVLV";
		static const char REVOLVER_TERM_LOAN[] = "RVLVTRM";
		static const char SECURITIES_LOAN[] = "SECLOAN";
		static const char SECURITIES_PLEDGE[] = "SECPLEDGE";
		static const char SPECIAL_ASSESSMENT[] = "SPCLA";
		static const char SPECIAL_OBLIGATION[] = "SPCLO";
		static const char SPECIAL_TAX[] = "SPCLT";
		static const char SHORT_TERM_LOAN_NOTE[] = "STN";
		static const char STRUCTURED_NOTES[] = "STRUCT";
		static const char USD_SUPRANATIONAL_COUPONS[] = "SUPRA";
		static const char SWING_LINE_FACILITY[] = "SWING";
		static const char TAX_ANTICIPATION_NOTE[] = "TAN";
		static const char TAX_ALLOCATION[] = "TAXA";
		static const char TO_BE_ANNOUNCED[] = "TBA";
		static const char US_TREASURY_BILL[] = "TBILL";
		static const char US_TREASURY_BOND[] = "TBOND";
		static const char PRINCIPAL_STRIP_OF_A_CALLABLE_BOND_OR_NOTE[] = "TCAL";
		static const char TIME_DEPOSIT[] = "TD";
		static const char TAX_EXEMPT_COMMERCIAL_PAPER[] = "TECP";
		static const char TERM_LOAN[] = "TERM";
		static const char INTEREST_STRIP_FROM_ANY_BOND_OR_NOTE[] = "TINT";
		static const char TREASURY_INFLATION_PROTECTED_SECURITIES[] = "TIPS";
		static const char US_TREASURY_NOTE[] = "TNOTE";
		static const char PRINCIPAL_STRIP_FROM_A_NON_CALLABLE_BOND_OR_NOTE[] = "TPRN";
		static const char TAX_AND_REVENUE_ANTICIPATION_NOTE[] = "TRAN";
		static const char VARIABLE_RATE_DEMAND_NOTE[] = "VRDN";
		static const char WARRANT[] = "WAR";
		static const char WITHDRAWN[] = "WITHDRN";
		static const char EXTENDED_COMM_NOTE[] = "XCN";
		static const char INDEXED_LINKED[] = "XLINKD";
		static const char YANKEE_CORPORATE_BOND[] = "YANK";
		static const char YANKEE_CERTIFICATE_OF_DEPOSIT[] = "YCD";
	};

	namespace StandInstDbType_val {
		static int OTHER = 0;
		static int DTC_SID = 1;
		static int THOMSON_ALERT = 2;
		static int A_GLOBAL_CUSTODIAN = 3;
		static int ACCOUNTNET = 4;
	};

	namespace StandInstDbName_val {
	};

	namespace StandInstDbID_val {
	};

	namespace SettlDeliveryType_val {
		static int VERSUS_PAYMENT = 0;
		static int FREE = 1;
		static int TRI_PARTY = 2;
		static int HOLD_IN_CUSTODY = 3;
	};

	namespace AllocLinkID_val {
	};

	namespace AllocLinkType_val {
		static int F_X_NETTING = 0;
		static int F_X_SWAP = 1;
	};

	namespace SecondaryOrderID_val {
	};

	namespace PutOrCall_val {
		static int PUT = 0;
		static int CALL = 1;
	};

	namespace CoveredOrUncovered_val {
		static int COVERED = 0;
		static int UNCOVERED = 1;
	};

	namespace OptAttribute_val {
	};

	namespace AllocHandlInst_val {
		static int MATCH = 1;
		static int FORWARD = 2;
		static int FORWARD_AND_MATCH = 3;
	};

	namespace SettlInstRefID_val {
	};

	namespace RoutingType_val {
		static int TARGET_FIRM = 1;
		static int TARGET_LIST = 2;
		static int BLOCK_FIRM = 3;
		static int BLOCK_LIST = 4;
	};

	namespace RoutingID_val {
	};

	namespace BenchmarkCurveName_val {
		static const char MUNIAAA[] = "MuniAAA";
		static const char FUTURESWAP[] = "FutureSWAP";
		static const char LIBID[] = "LIBID";
		static const char LIBOR[] = "LIBOR";
		static const char OTHER[] = "OTHER";
		static const char SWAP[] = "SWAP";
		static const char TREASURY[] = "Treasury";
		static const char EURIBOR[] = "Euribor";
		static const char PFANDBRIEFE[] = "Pfandbriefe";
		static const char EONIA[] = "EONIA";
		static const char SONIA[] = "SONIA";
		static const char EUREPO[] = "EUREPO";
	};

	namespace BenchmarkCurvePoint_val {
	};

	namespace RepurchaseTerm_val {
	};

	namespace StipulationType_val {
		static const char AMT[] = "AMT";
		static const char AUTO_REINVESTMENT_AT_OR_BETTER[] = "AUTOREINV";
		static const char BANK_QUALIFIED[] = "BANKQUAL";
		static const char BARGAIN_CONDITIONS[] = "BGNCON";
		static const char COUPON_RANGE[] = "COUPON";
		static const char ISO_CURRENCY_CODE[] = "CURRENCY";
		static const char CUSTOM_START_END_DATE[] = "CUSTOMDATE";
		static const char GEOGRAPHICS_AND_PERCENT_RANGE[] = "GEOG";
		static const char VALUATION_DISCOUNT[] = "HAIRCUT";
		static const char INSURED[] = "INSURED";
		static const char YEAR_OR_YEAR_MONTH_OF_ISSUE[] = "ISSUE";
		static const char ISSUERS_TICKER[] = "ISSUER";
		static const char ISSUE_SIZE_RANGE[] = "ISSUESIZE";
		static const char LOOKBACK_DAYS[] = "LOOKBACK";
		static const char EXPLICIT_LOT_IDENTIFIER[] = "LOT";
		static const char LOT_VARIANCE[] = "LOTVAR";
		static const char MATURITY_YEAR_AND_MONTH[] = "MAT";
		static const char MATURITY_RANGE[] = "MATURITY";
		static const char MAXIMUM_SUBSTITUTIONS[] = "MAXSUBS";
		static const char MINIMUM_QUANTITY[] = "MINQTY";
		static const char MINIMUM_INCREMENT[] = "MININCR";
		static const char MINIMUM_DENOMINATION[] = "MINDNOM";
		static const char PAYMENT_FREQUENCY_CALENDAR[] = "PAYFREQ";
		static const char NUMBER_OF_PIECES[] = "PIECES";
		static const char POOLS_MAXIMUM[] = "PMAX";
		static const char POOLS_PER_MILLION[] = "PPM";
		static const char POOLS_PER_LOT[] = "PPL";
		static const char POOLS_PER_TRADE[] = "PPT";
		static const char PRICE_RANGE[] = "PRICE";
		static const char PRICING_FREQUENCY[] = "PRICEFREQ";
		static const char PRODUCTION_YEAR[] = "PROD";
		static const char CALL_PROTECTION[] = "PROTECT";
		static const char PURPOSE[] = "PURPOSE";
		static const char BENCHMARK_PRICE_SOURCE[] = "PXSOURCE";
		static const char RATING_SOURCE_AND_RANGE[] = "RATING";
		static const char RESTRICTED[] = "RESTRICTED";
		static const char MARKET_SECTOR[] = "SECTOR";
		static const char SECURITYTYPE_INCLUDED_OR_EXCLUDED[] = "SECTYPE";
		static const char STRUCTURE[] = "STRUCT";
		static const char SUBSTITUTIONS_FREQUENCY[] = "SUBSFREQ";
		static const char SUBSTITUTIONS_LEFT[] = "SUBSLEFT";
		static const char FREEFORM_TEXT[] = "TEXT";
		static const char TRADE_VARIANCE[] = "TRDVAR";
		static const char WEIGHTED_AVERAGE_COUPON[] = "WAC";
		static const char WEIGHTED_AVERAGE_LIFE_COUPON[] = "WAL";
		static const char WEIGHTED_AVERAGE_LOAN_AGE[] = "WALA";
		static const char WEIGHTED_AVERAGE_MATURITY[] = "WAM";
		static const char WHOLE_POOL[] = "WHOLE";
		static const char YIELD_RANGE[] = "YIELD";
		static const char SINGLE_MONTHLY_MORTALITY[] = "SMM";
		static const char CONSTANT_PREPAYMENT_RATE[] = "CPR";
		static const char CONSTANT_PREPAYMENT_YIELD[] = "CPY";
		static const char CONSTANT_PREPAYMENT_PENALTY[] = "CPP";
		static const char ABSOLUTE_PREPAYMENT_SPEED[] = "ABS";
		static const char MONTHLY_PREPAYMENT_RATE[] = "MPR";
		static const char PERCENT_OF_BMA_PREPAYMENT_CURVE[] = "PSA";
		static const char PERCENT_OF_PROSPECTUS_PREPAYMENT_CURVE[] = "PPC";
		static const char PERCENT_OF_MANUFACTURED_HOUSING_PREPAYMENT_CURVE[] = "MHP";
		static const char FINAL_CPR_OF_HOME_EQUITY_PREPAYMENT_CURVE[] = "HEP";
	};

	namespace StipulationValue_val {
		static const char SPECIAL_CUM_DIVIDEND[] = "CD";
		static const char SPECIAL_EX_DIVIDEND[] = "XD";
		static const char SPECIAL_CUM_COUPON[] = "CC";
		static const char SPECIAL_EX_COUPON[] = "XC";
		static const char SPECIAL_CUM_BONUS[] = "CB";
		static const char SPECIAL_EX_BONUS[] = "XB";
		static const char SPECIAL_CUM_RIGHTS[] = "CR";
		static const char SPECIAL_EX_RIGHTS[] = "XR";
		static const char SPECIAL_CUM_CAPITAL_REPAYMENTS[] = "CP";
		static const char SPECIAL_EX_CAPITAL_REPAYMENTS[] = "XP";
		static const char CASH_SETTLEMENT[] = "CS";
		static const char SPECIAL_PRICE[] = "SP";
		static const char REPORT_FOR_EUROPEAN_EQUITY_MARKET_SECURITIES[] = "TR";
		static const char GUARANTEED_DELIVERY[] = "GD";
	};

	namespace YieldType_val {
		static const char AFTER_TAX_YIELD[] = "AFTERTAX";
		static const char ANNUAL_YIELD[] = "ANNUAL";
		static const char YIELD_AT_ISSUE[] = "ATISSUE";
		static const char YIELD_TO_AVERAGE_MATURITY[] = "AVGMATURITY";
		static const char BOOK_YIELD[] = "BOOK";
		static const char YIELD_TO_NEXT_CALL[] = "CALL";
		static const char YIELD_CHANGE_SINCE_CLOSE[] = "CHANGE";
		static const char CLOSING_YIELD[] = "CLOSE";
		static const char COMPOUND_YIELD[] = "COMPOUND";
		static const char CURRENT_YIELD[] = "CURRENT";
		static const char TRUE_GROSS_YIELD[] = "GROSS";
		static const char GOVERNMENT_EQUIVALENT_YIELD[] = "GOVTEQUIV";
		static const char YIELD_WITH_INFLATION_ASSUMPTION[] = "INFLATION";
		static const char INVERSE_FLOATER_BOND_YIELD[] = "INVERSEFLOATER";
		static const char MOST_RECENT_CLOSING_YIELD[] = "LASTCLOSE";
		static const char CLOSING_YIELD_MOST_RECENT_MONTH[] = "LASTMONTH";
		static const char CLOSING_YIELD_MOST_RECENT_QUARTER[] = "LASTQUARTER";
		static const char CLOSING_YIELD_MOST_RECENT_YEAR[] = "LASTYEAR";
		static const char YIELD_TO_LONGEST_AVERAGE_LIFE[] = "LONGAVGLIFE";
		static const char MARK_TO_MARKET_YIELD[] = "MARK";
		static const char YIELD_TO_MATURITY[] = "MATURITY";
		static const char YIELD_TO_NEXT_REFUND[] = "NEXTREFUND";
		static const char OPEN_AVERAGE_YIELD[] = "OPENAVG";
		static const char YIELD_TO_NEXT_PUT[] = "PUT";
		static const char PREVIOUS_CLOSE_YIELD[] = "PREVCLOSE";
		static const char PROCEEDS_YIELD[] = "PROCEEDS";
		static const char SEMI_ANNUAL_YIELD[] = "SEMIANNUAL";
		static const char YIELD_TO_SHORTEST_AVERAGE_LIFE[] = "SHORTAVGLIFE";
		static const char SIMPLE_YIELD[] = "SIMPLE";
		static const char TAX_EQUIVALENT_YIELD[] = "TAXEQUIV";
		static const char YIELD_TO_TENDER_DATE[] = "TENDER";
		static const char TRUE_YIELD[] = "TRUE";
		static const char YIELD_VALUE_OF_1_32[] = "VALUE1_32";
		static const char YIELD_TO_WORST[] = "WORST";
	};

	namespace RepoCollateralSecurityType_val {
	};

	namespace UnderlyingRepoCollateralSecurityType_val {
	};

	namespace UnderlyingRepurchaseTerm_val {
	};

	namespace LegRepoCollateralSecurityType_val {
	};

	namespace LegRepurchaseTerm_val {
	};

	namespace CreditRating_val {
	};

	namespace UnderlyingCreditRating_val {
	};

	namespace LegCreditRating_val {
	};

	namespace MDReqID_val {
	};

	namespace SubscriptionRequestType_val {
		static const char SNAPSHOT[] = "0";
		static const char SNAPSHOT_PLUS_UPDATES[] = "1";
		static const char DISABLE_PREVIOUS_SNAPSHOT_PLUS_UPDATE_REQUEST[] = "2";
	};

	namespace MarketDepth_val {
		static int FULL_BOOK = 0;
		static int TOP_OF_BOOK = 1;
	};

	namespace MDUpdateType_val {
		static int FULL_REFRESH = 0;
		static int INCREMENTAL_REFRESH = 1;
	};

	namespace MDEntryType_val {
		static const char BID[] = "0";
		static const char OFFER[] = "1";
		static const char TRADE[] = "2";
		static const char INDEX_VALUE[] = "3";
		static const char OPENING_PRICE[] = "4";
		static const char CLOSING_PRICE[] = "5";
		static const char SETTLEMENT_PRICE[] = "6";
		static const char TRADING_SESSION_HIGH_PRICE[] = "7";
		static const char TRADING_SESSION_LOW_PRICE[] = "8";
		static const char TRADING_SESSION_VWAP_PRICE[] = "9";
		static const char IMBALANCE[] = "A";
		static const char TRADE_VOLUME[] = "B";
		static const char OPEN_INTEREST[] = "C";
		static const char AUCTION_CLEARING_PRICE[] = "Q";
	};

	namespace TickDirection_val {
		static const char PLUS_TICK[] = "0";
		static const char ZERO_PLUS_TICK[] = "1";
		static const char MINUS_TICK[] = "2";
		static const char ZERO_MINUS_TICK[] = "3";
	};

	namespace MDEntryID_val {
	};

	namespace MDUpdateAction_val {
		static const char NEW[] = "0";
		static const char CHANGE[] = "1";
		static const char DELETE[] = "2";
	};

	namespace MDEntryRefID_val {
	};

	namespace MDReqRejReason_val {
		static const char UNKNOWN_SYMBOL[] = "0";
		static const char DUPLICATE_MDREQID[] = "1";
		static const char INSUFFICIENT_BANDWIDTH[] = "2";
		static const char INSUFFICIENT_PERMISSIONS[] = "3";
		static const char UNSUPPORTED_SUBSCRIPTIONREQUESTTYPE[] = "4";
		static const char UNSUPPORTED_MARKETDEPTH[] = "5";
		static const char UNSUPPORTED_MDUPDATETYPE[] = "6";
		static const char UNSUPPORTED_AGGREGATEDBOOK[] = "7";
		static const char UNSUPPORTED_MDENTRYTYPE[] = "8";
		static const char UNSUPPORTED_TRADINGSESSIONID[] = "9";
		static const char UNSUPPORTED_SCOPE[] = "A";
		static const char UNSUPPORTED_OPENCLOSESETTLEFLAG[] = "B";
		static const char UNSUPPORTED_MDIMPLICITDELETE[] = "C";
	};

	namespace MDEntryOriginator_val {
	};

	namespace LocationID_val {
	};

	namespace DeskID_val {
	};

	namespace DeleteReason_val {
		static const char CANCELATION_TRADE_BUST[] = "0";
		static const char ERROR[] = "1";
	};

	namespace SellerDays_val {
	};

	namespace MDEntryBuyer_val {
	};

	namespace MDEntrySeller_val {
	};

	namespace MDEntryPositionNo_val {
	};

	namespace QuoteStatus_val {
		static int ACCEPTED = 0;
		static int CANCELED_FOR_SYMBOL = 1;
		static int CANCELED_FOR_SECURITY_TYPE = 2;
		static int CANCELED_FOR_UNDERLYING = 3;
		static int CANCELED_ALL = 4;
		static int REJECTED = 5;
		static int REMOVED_FROM_MARKET = 6;
		static int EXPIRED = 7;
		static int QUERY = 8;
		static int QUOTE_NOT_FOUND = 9;
		static int PENDING = 10;
		static int PASS = 11;
		static int LOCKED_MARKET_WARNING = 12;
		static int CROSS_MARKET_WARNING = 13;
		static int CANCELED_DUE_TO_LOCK_MARKET = 14;
		static int CANCELED_DUE_TO_CROSS_MARKET = 15;
	};

	namespace QuoteCancelType_val {
		static int CANCEL_FOR_SYMBOL = 1;
		static int CANCEL_FOR_SECURITY_TYPE = 2;
		static int CANCEL_FOR_UNDERLYING_SYMBOL = 3;
		static int CANCEL_ALL_QUOTES = 4;
	};

	namespace QuoteEntryID_val {
	};

	namespace QuoteRejectReason_val {
		static int UNKNOWN_SYMBOL = 1;
		static int EXCHANGE_CLOSED = 2;
		static int QUOTE_REQUEST_EXCEEDS_LIMIT = 3;
		static int TOO_LATE_TO_ENTER = 4;
		static int UNKNOWN_QUOTE = 5;
		static int DUPLICATE_QUOTE = 6;
		static int INVALID_BID_ASK_SPREAD = 7;
		static int INVALID_PRICE = 8;
		static int NOT_AUTHORIZED_TO_QUOTE_SECURITY = 9;
		static int OTHER = 99;
	};

	namespace QuoteResponseLevel_val {
		static int NO_ACKNOWLEDGEMENT = 0;
		static int ACKNOWLEDGE_ONLY_NEGATIVE_OR_ERRONEOUS_QUOTES = 1;
		static int ACKNOWLEDGE_EACH_QUOTE_MESSAGES = 2;
	};

	namespace QuoteSetID_val {
	};

	namespace QuoteRequestType_val {
		static int MANUAL = 1;
		static int AUTOMATIC = 2;
	};

	namespace TotNoQuoteEntries_val {
	};

	namespace UnderlyingSecurityIDSource_val {
	};

	namespace UnderlyingIssuer_val {
	};

	namespace UnderlyingSecurityDesc_val {
	};

	namespace UnderlyingSecurityID_val {
	};

	namespace UnderlyingSecurityType_val {
	};

	namespace UnderlyingSymbol_val {
	};

	namespace UnderlyingSymbolSfx_val {
	};

	namespace UnderlyingPutOrCall_val {
		static int PUT = 0;
		static int CALL = 1;
	};

	namespace UnderlyingOptAttribute_val {
	};

	namespace SecurityReqID_val {
	};

	namespace SecurityRequestType_val {
		static int REQUEST_SECURITY_IDENTITY_AND_SPECIFICATIONS = 0;
		static int REQUEST_SECURITY_IDENTITY_FOR_THE_SPECIFICATIONS_PROVIDED = 1;
		static int REQUEST_LIST_SECURITY_TYPES = 2;
		static int REQUEST_LIST_SECURITIES = 3;
	};

	namespace SecurityResponseID_val {
	};

	namespace SecurityResponseType_val {
		static int ACCEPT_SECURITY_PROPOSAL_AS_IS = 1;
		static int ACCEPT_SECURITY_PROPOSAL_WITH_REVISIONS_AS_INDICATED_IN_THE_MESSAGE = 2;
		static int LIST_OF_SECURITY_TYPES_RETURNED_PER_REQUEST = 3;
		static int LIST_OF_SECURITIES_RETURNED_PER_REQUEST = 4;
		static int REJECT_SECURITY_PROPOSAL = 5;
		static int CAN_NOT_MATCH_SELECTION_CRITERIA = 6;
	};

	namespace SecurityStatusReqID_val {
	};

	namespace SecurityTradingStatus_val {
		static int OPENING_DELAY = 1;
		static int TRADING_HALT = 2;
		static int RESUME = 3;
		static int NO_OPEN_NO_RESUME = 4;
		static int PRICE_INDICATION = 5;
		static int TRADING_RANGE_INDICATION = 6;
		static int MARKET_IMBALANCE_BUY = 7;
		static int MARKET_IMBALANCE_SELL = 8;
		static int MARKET_ON_CLOSE_IMBALANCE_BUY = 9;
		static int MARKET_ON_CLOSE_IMBALANCE_SELL = 10;
		static int NOT_ASSIGNED = 11;
		static int NO_MARKET_IMBALANCE = 12;
		static int NO_MARKET_ON_CLOSE_IMBALANCE = 13;
		static int ITS_PRE_OPENING = 14;
		static int NEW_PRICE_INDICATION = 15;
		static int TRADE_DISSEMINATION_TIME = 16;
		static int READY_TO_TRADE_START_OF_SESSION = 17;
		static int NOT_AVAILABLE_FOR_TRADING_END_OF_SESSION = 18;
		static int NOT_TRADED_ON_THIS_MARKET = 19;
		static int UNKNOWN_OR_INVALID = 20;
		static int PRE_OPEN = 21;
		static int OPENING_ROTATION = 22;
		static int FAST_MARKET = 23;
	};

	namespace HaltReason_val {
		static const char ORDER_IMBALANCE[] = "I";
		static const char EQUIPMENT_CHANGEOVER[] = "X";
		static const char NEWS_PENDING[] = "P";
		static const char NEWS_DISSEMINATION[] = "D";
		static const char ORDER_INFLUX[] = "E";
		static const char ADDITIONAL_INFORMATION[] = "M";
	};

	namespace Adjustment_val {
		static int CANCEL = 1;
		static int ERROR = 2;
		static int CORRECTION = 3;
	};

	namespace TradSesReqID_val {
	};

	namespace TradingSessionID_val {
	};

	namespace ContraTrader_val {
	};

	namespace TradSesMethod_val {
		static int ELECTRONIC = 1;
		static int OPEN_OUTCRY = 2;
		static int TWO_PARTY = 3;
	};

	namespace TradSesMode_val {
		static int TESTING = 1;
		static int SIMULATED = 2;
		static int PRODUCTION = 3;
	};

	namespace TradSesStatus_val {
		static int UNKNOWN = 0;
		static int HALTED = 1;
		static int OPEN = 2;
		static int CLOSED = 3;
		static int PRE_OPEN = 4;
		static int PRE_CLOSE = 5;
		static int REQUEST_REJECTED = 6;
	};

	namespace NumberOfOrders_val {
	};

	namespace MessageEncoding_val {
		static const char ISO_2022_JP[] = "ISO-2022-JP";
		static const char EUC_JP[] = "EUC-JP";
		static const char SHIFT_JIS[] = "SHIFT_JIS";
		static const char UTF_8[] = "UTF-8";
	};

	namespace QuoteEntryRejectReason_val {
		static int UNKNOWN_SYMBOL = 1;
		static int EXCHANGE_CLOSED = 2;
		static int QUOTE_EXCEEDS_LIMIT = 3;
		static int TOO_LATE_TO_ENTER = 4;
		static int UNKNOWN_QUOTE = 5;
		static int DUPLICATE_QUOTE = 6;
		static int INVALID_BID_ASK_SPREAD = 7;
		static int INVALID_PRICE = 8;
		static int NOT_AUTHORIZED_TO_QUOTE_SECURITY = 9;
	};

	namespace RefTagID_val {
	};

	namespace RefMsgType_val {
	};

	namespace SessionRejectReason_val {
		static int INVALID_TAG_NUMBER = 0;
		static int REQUIRED_TAG_MISSING = 1;
		static int TAG_NOT_DEFINED_FOR_THIS_MESSAGE_TYPE = 2;
		static int UNDEFINED_TAG = 3;
		static int TAG_SPECIFIED_WITHOUT_A_VALUE = 4;
		static int VALUE_IS_INCORRECT = 5;
		static int INCORRECT_DATA_FORMAT_FOR_VALUE = 6;
		static int DECRYPTION_PROBLEM = 7;
		static int SIGNATURE_PROBLEM = 8;
		static int COMPID_PROBLEM = 9;
		static int SENDINGTIME_ACCURACY_PROBLEM = 10;
		static int INVALID_MSGTYPE = 11;
		static int XML_VALIDATION_ERROR = 12;
		static int TAG_APPEARS_MORE_THAN_ONCE = 13;
		static int TAG_SPECIFIED_OUT_OF_REQUIRED_ORDER = 14;
		static int REPEATING_GROUP_FIELDS_OUT_OF_ORDER = 15;
		static int INCORRECT_NUMINGROUP_COUNT_FOR_REPEATING_GROUP = 16;
		static int NON_DATA_VALUE_INCLUDES_FIELD_DELIMITER = 17;
		static int OTHER = 99;
	};

	namespace BidRequestTransType_val {
		static const char NEW[] = "N";
		static const char CANCEL[] = "C";
	};

	namespace ContraBroker_val {
	};

	namespace ComplianceID_val {
	};

	namespace ExecRestatementReason_val {
		static int GT_CORPORATE_ACTION = 0;
		static int GT_RENEWAL_RESTATEMENT = 1;
		static int VERBAL_CHANGE = 2;
		static int REPRICING_OF_ORDER = 3;
		static int BROKER_OPTION = 4;
		static int PARTIAL_DECLINE_OF_ORDERQTY = 5;
		static int CANCEL_ON_TRADING_HALT = 6;
		static int CANCEL_ON_SYSTEM_FAILURE = 7;
		static int MARKET_OPTION = 8;
		static int CANCELED_NOT_BEST = 9;
		static int WAREHOUSE_RECAP = 10;
		static int OTHER = 99;
	};

	namespace BusinessRejectRefID_val {
	};

	namespace BusinessRejectReason_val {
		static int OTHER = 0;
		static int UNKOWN_ID = 1;
		static int UNKNOWN_SECURITY = 2;
		static int UNSUPPORTED_MESSAGE_TYPE = 3;
		static int APPLICATION_NOT_AVAILABLE = 4;
		static int CONDITIONALLY_REQUIRED_FIELD_MISSING = 5;
		static int NOT_AUTHORIZED = 6;
		static int DELIVERTO_FIRM_NOT_AVAILABLE_AT_THIS_TIME = 7;
	};

	namespace MsgDirection_val {
		static const char SEND[] = "S";
		static const char RECEIVE[] = "R";
	};

	namespace DiscretionInst_val {
		static const char RELATED_TO_DISPLAYED_PRICE[] = "0";
		static const char RELATED_TO_MARKET_PRICE[] = "1";
		static const char RELATED_TO_PRIMARY_PRICE[] = "2";
		static const char RELATED_TO_LOCAL_PRIMARY_PRICE[] = "3";
		static const char RELATED_TO_MIDPOINT_PRICE[] = "4";
		static const char RELATED_TO_LAST_TRADE_PRICE[] = "5";
		static const char RELATED_TO_VWAP[] = "6";
	};

	namespace BidID_val {
	};

	namespace ClientBidID_val {
	};

	namespace ListName_val {
	};

	namespace TotNoRelatedSym_val {
	};

	namespace BidType_val {
		static int NON_DISCLOSED = 1;
		static int DISCLOSED_STYLE = 2;
		static int NO_BIDDING_PROCESS = 3;
	};

	namespace NumTickets_val {
	};

	namespace BidDescriptorType_val {
		static int SECTOR = 1;
		static int COUNTRY = 2;
		static int INDEX = 3;
	};

	namespace BidDescriptor_val {
	};

	namespace SideValueInd_val {
		static int SIDEVALUE1 = 1;
		static int SIDEVALUE2 = 2;
	};

	namespace LiquidityIndType_val {
		static int FIVEDAY_MOVING_AVERAGE = 1;
		static int TWENTYDAY_MOVING_AVERAGE = 2;
		static int NORMAL_MARKET_SIZE = 3;
		static int OTHER = 4;
	};

	namespace ProgRptReqs_val {
		static int BUYSIDE_EXPLICITLY_REQUESTS_STATUS_USING_STATUSREQUEST = 1;
		static int SELLSIDE_PERIODICALLY_SENDS_STATUS_USING_LISTSTATUS = 2;
		static int REAL_TIME_EXECUTION_REPORTS = 3;
	};

	namespace ProgPeriodInterval_val {
	};

	namespace IncTaxInd_val {
		static int NET = 1;
		static int GROSS = 2;
	};

	namespace NumBidders_val {
	};

	namespace BidTradeType_val {
		static const char RISK_TRADE[] = "R";
		static const char VWAP_GUARANTEE[] = "G";
		static const char AGENCY[] = "A";
		static const char GUARANTEED_CLOSE[] = "J";
	};

	namespace BasisPxType_val {
		static const char CLOSING_PRICE_AT_MORNING_SESSION[] = "2";
		static const char CLOSING_PRICE[] = "3";
		static const char CURRENT_PRICE[] = "4";
		static const char SQ[] = "5";
		static const char VWAP_THROUGH_A_DAY[] = "6";
		static const char VWAP_THROUGH_A_MORNING_SESSION[] = "7";
		static const char VWAP_THROUGH_AN_AFTERNOON_SESSION[] = "8";
		static const char VWAP_THROUGH_A_DAY_EXCEPT_YORI[] = "9";
		static const char VWAP_THROUGH_A_MORNING_SESSION_EXCEPT_YORI[] = "A";
		static const char VWAP_THROUGH_AN_AFTERNOON_SESSION_EXCEPT_YORI[] = "B";
		static const char STRIKE[] = "C";
		static const char OPEN[] = "D";
		static const char OTHERS[] = "Z";
	};

	namespace TotNoStrikes_val {
	};

	namespace PriceType_val {
		static int PERCENTAGE = 1;
		static int PER_UNIT = 2;
		static int FIXED_AMOUNT = 3;
		static int DISCOUNT = 4;
		static int PREMIUM = 5;
		static int SPREAD = 6;
		static int TED_PRICE = 7;
		static int TED_YIELD = 8;
		static int YIELD = 9;
		static int FIXED_CABINET_TRADE_PRICE = 10;
		static int VARIABLE_CABINET_TRADE_PRICE = 11;
	};

	namespace GTBookingInst_val {
		static int BOOK_OUT_ALL_TRADES_ON_DAY_OF_EXECUTION = 0;
		static int ACCUMULATE_EXECUTIONS_UNTIL_ORDER_IS_FILLED_OR_EXPIRES = 1;
		static int ACCUMULATE_UNTIL_VERBALLY_NOTIFIED_OTHERWISE = 2;
	};

	namespace ListStatusType_val {
		static int ACK = 1;
		static int RESPONSE = 2;
		static int TIMED = 3;
		static int EXECSTARTED = 4;
		static int ALLDONE = 5;
		static int ALERT = 6;
	};

	namespace NetGrossInd_val {
		static int NET = 1;
		static int GROSS = 2;
	};

	namespace ListOrderStatus_val {
		static int INBIDDINGPROCESS = 1;
		static int RECEIVEDFOREXECUTION = 2;
		static int EXECUTING = 3;
		static int CANCELING = 4;
		static int ALERT = 5;
		static int ALL_DONE = 6;
		static int REJECT = 7;
	};

	namespace ListExecInstType_val {
		static const char IMMEDIATE[] = "1";
		static const char WAIT_FOR_EXECUTE_INSTRUCTION[] = "2";
		static const char EXCHANGE_SWITCH_CIV_ORDER_SELL_DRIVEN[] = "3";
		static const char EXCHANGE_SWITCH_CIV_ORDER_BUY_DRIVEN_CASH_TOP_UP[] = "4";
		static const char EXCHANGE_SWITCH_CIV_ORDER_BUY_DRIVEN_CASH_WITHDRAW[] = "5";
	};

	namespace CxlRejResponseTo_val {
		static const char ORDER_CANCEL_REQUEST[] = "1";
		static const char ORDER_CANCEL_REPLACE_REQUEST[] = "2";
	};

	namespace LiquidityNumSecurities_val {
	};

	namespace MultiLegReportingType_val {
		static const char SINGLE_SECURITY[] = "1";
		static const char INDIVIDUAL_LEG_OF_A_MULTI_LEG_SECURITY[] = "2";
		static const char MULTI_LEG_SECURITY[] = "3";
	};

	namespace ListStatusText_val {
	};

	namespace PartyIDSource_val {
		static const char BIC[] = "B";
		static const char GENERALLY_ACCEPTED_MARKET_PARTICIPANT_IDENTIFIER[] = "C";
		static const char PROPRIETARY_CUSTOM_CODE[] = "D";
		static const char ISO_COUNTRY_CODE[] = "E";
		static const char SETTLEMENT_ENTITY_LOCATION[] = "F";
		static const char MIC[] = "G";
		static const char CSD_PARTICIPANT_MEMBER_CODE[] = "H";
		static const char KOREAN_INVESTOR_ID[] = "1";
		static const char TAIWANESE_QUALIFIED_FOREIGN_INVESTOR_ID_QFII_FID[] = "2";
		static const char TAIWANESE_TRADING_ACCOUNT[] = "3";
		static const char MALAYSIAN_CENTRAL_DEPOSITORY_NUMBER[] = "4";
		static const char CHINESE_B_SHARE[] = "5";
		static const char UK_NATIONAL_INSURANCE_OR_PENSION_NUMBER[] = "6";
		static const char US_SOCIAL_SECURITY_NUMBER[] = "7";
		static const char US_EMPLOYER_IDENTIFICATION_NUMBER[] = "8";
		static const char AUSTRALIAN_BUSINESS_NUMBER[] = "9";
		static const char AUSTRALIAN_TAX_FILE_NUMBER[] = "A";
		static const char DIRECTED_BROKER[] = "I";
	};

	namespace PartyID_val {
	};

	namespace PartyRole_val {
		static int EXECUTING_FIRM = 1;
		static int BROKER_OF_CREDIT = 2;
		static int CLIENT_ID = 3;
		static int CLEARING_FIRM = 4;
		static int INVESTOR_ID = 5;
		static int INTRODUCING_FIRM = 6;
		static int ENTERING_FIRM = 7;
		static int LOCATE_LENDING_FIRM = 8;
		static int FUND_MANAGER_CLIENT_ID = 9;
		static int SETTLEMENT_LOCATION = 10;
		static int ORDER_ORIGINATION_TRADER = 11;
		static int EXECUTING_TRADER = 12;
		static int ORDER_ORIGINATION_FIRM = 13;
		static int GIVEUP_CLEARING_FIRM = 14;
		static int CORRESPONDANT_CLEARING_FIRM = 15;
		static int EXECUTING_SYSTEM = 16;
		static int CONTRA_FIRM = 17;
		static int CONTRA_CLEARING_FIRM = 18;
		static int SPONSORING_FIRM = 19;
		static int UNDERLYING_CONTRA_FIRM = 20;
		static int CLEARING_ORGANIZATION = 21;
		static int EXCHANGE = 22;
		static int CUSTOMER_ACCOUNT = 24;
		static int CORRESPONDENT_CLEARING_ORGANIZATION = 25;
		static int CORRESPONDENT_BROKER = 26;
		static int BUYER_SELLER = 27;
		static int CUSTODIAN = 28;
		static int INTERMEDIARY = 29;
		static int AGENT = 30;
		static int SUB_CUSTODIAN = 31;
		static int BENEFICIARY = 32;
		static int INTERESTED_PARTY = 33;
		static int REGULATORY_BODY = 34;
		static int LIQUIDITY_PROVIDER = 35;
		static int ENTERING_TRADER = 36;
		static int CONTRA_TRADER = 37;
		static int POSITION_ACCOUNT = 38;
	};

	namespace SecurityAltID_val {
	};

	namespace SecurityAltIDSource_val {
	};

	namespace UnderlyingSecurityAltID_val {
	};

	namespace UnderlyingSecurityAltIDSource_val {
	};

	namespace Product_val {
		static int AGENCY = 1;
		static int COMMODITY = 2;
		static int CORPORATE = 3;
		static int CURRENCY = 4;
		static int EQUITY = 5;
		static int GOVERNMENT = 6;
		static int INDEX = 7;
		static int LOAN = 8;
		static int MONEYMARKET = 9;
		static int MORTGAGE = 10;
		static int MUNICIPAL = 11;
		static int OTHER = 12;
		static int FINANCING = 13;
	};

	namespace CFICode_val {
	};

	namespace UnderlyingProduct_val {
	};

	namespace UnderlyingCFICode_val {
	};

	namespace QuantityType_val {
		static int SHARES = 1;
		static int BONDS = 2;
		static int CURRENTFACE = 3;
		static int ORIGINALFACE = 4;
		static int CURRENCY = 5;
		static int CONTRACTS = 6;
		static int OTHER = 7;
		static int PAR = 8;
	};

	namespace BookingRefID_val {
	};

	namespace IndividualAllocID_val {
	};

	namespace RoundingDirection_val {
		static const char ROUND_TO_NEAREST[] = "0";
		static const char ROUND_DOWN[] = "1";
		static const char ROUND_UP[] = "2";
	};

	namespace StateOrProvinceOfIssue_val {
	};

	namespace LocaleOfIssue_val {
	};

	namespace MailingDtls_val {
	};

	namespace PaymentRef_val {
	};

	namespace DistribPaymentMethod_val {
		static int CREST = 1;
		static int NSCC = 2;
		static int EUROCLEAR = 3;
		static int CLEARSTREAM = 4;
		static int CHEQUE = 5;
		static int TELEGRAPHIC_TRANSFER = 6;
		static int FEDWIRE = 7;
		static int DIRECT_CREDIT = 8;
		static int ACH_CREDIT = 9;
		static int BPAY = 10;
		static int HIGH_VALUE_CLEARING_SYSTEM = 11;
		static int REINVEST_IN_FUND = 12;
	};

	namespace CancellationRights_val {
		static const char YES[] = "Y";
		static const char NO_EXECUTION_ONLY[] = "N";
		static const char NO_WAIVER_AGREEMENT[] = "M";
		static const char NO_INSTITUTIONAL[] = "O";
	};

	namespace MoneyLaunderingStatus_val {
		static const char PASSED[] = "Y";
		static const char NOT_CHECKED[] = "N";
		static const char EXEMPT_BELOW_THE_LIMIT[] = "1";
		static const char EXEMPT_CLIENT_MONEY_TYPE_EXEMPTION[] = "2";
		static const char EXEMPT_AUTHORISED_CREDIT_OR_FINANCIAL_INSTITUTION[] = "3";
	};

	namespace MailingInst_val {
	};

	namespace ExecPriceType_val {
		static const char BID_PRICE[] = "B";
		static const char CREATION_PRICE[] = "C";
		static const char CREATION_PRICE_PLUS_ADJUSTMENT_PERCENT[] = "D";
		static const char CREATION_PRICE_PLUS_ADJUSTMENT_AMOUNT[] = "E";
		static const char OFFER_PRICE[] = "O";
		static const char OFFER_PRICE_MINUS_ADJUSTMENT_PERCENT[] = "P";
		static const char OFFER_PRICE_MINUS_ADJUSTMENT_AMOUNT[] = "Q";
		static const char SINGLE_PRICE[] = "S";
	};

	namespace TradeReportTransType_val {
		static int NEW = 0;
		static int CANCEL = 1;
		static int REPLACE = 2;
		static int RELEASE = 3;
		static int REVERSE = 4;
	};

	namespace CardHolderName_val {
	};

	namespace CardNumber_val {
	};

	namespace CardIssNum_val {
	};

	namespace PaymentMethod_val {
		static int CREST = 1;
		static int NSCC = 2;
		static int EUROCLEAR = 3;
		static int CLEARSTREAM = 4;
		static int CHEQUE = 5;
		static int TELEGRAPHIC_TRANSFER = 6;
		static int FEDWIRE = 7;
		static int DEBIT_CARD = 8;
		static int DIRECT_DEBIT = 9;
		static int DIRECT_CREDIT = 10;
		static int CREDIT_CARD = 11;
		static int ACH_DEBIT = 12;
		static int ACH_CREDIT = 13;
		static int BPAY = 14;
		static int HIGH_VALUE_CLEARING_SYSTEM = 15;
	};

	namespace RegistAcctType_val {
	};

	namespace Designation_val {
	};

	namespace TaxAdvantageType_val {
		static int NONE = 0;
		static int MAXI_ISA = 1;
		static int TESSA = 2;
		static int MINI_CASH_ISA = 3;
		static int MINI_STOCKS_AND_SHARES_ISA = 4;
		static int MINI_INSURANCE_ISA = 5;
		static int CURRENT_YEAR_PAYMENT = 6;
		static int PRIOR_YEAR_PAYMENT = 7;
		static int ASSET_TRANSFER = 8;
		static int EMPLOYEE_PRIOR_YEAR = 9;
		static int OTHER = 999;
	};

	namespace RegistRejReasonText_val {
	};

	namespace FundRenewWaiv_val {
		static const char YES[] = "Y";
		static const char NO[] = "N";
	};

	namespace CashDistribAgentName_val {
	};

	namespace CashDistribAgentCode_val {
	};

	namespace CashDistribAgentAcctNumber_val {
	};

	namespace CashDistribPayRef_val {
	};

	namespace CashDistribAgentAcctName_val {
	};

	namespace PaymentRemitterID_val {
	};

	namespace RegistStatus_val {
		static const char ACCEPTED[] = "A";
		static const char REJECTED[] = "R";
		static const char HELD[] = "H";
		static const char REMINDER[] = "N";
	};

	namespace RegistRejReasonCode_val {
		static int INVALID_UNACCEPTABLE_ACCOUNT_TYPE = 1;
		static int INVALID_UNACCEPTABLE_TAX_EXEMPT_TYPE = 2;
		static int INVALID_UNACCEPTABLE_OWNERSHIP_TYPE = 3;
		static int INVALID_UNACCEPTABLE_NO_REG_DETLS = 4;
		static int INVALID_UNACCEPTABLE_REG_SEQ_NO = 5;
		static int INVALID_UNACCEPTABLE_REG_DTLS = 6;
		static int INVALID_UNACCEPTABLE_MAILING_DTLS = 7;
		static int INVALID_UNACCEPTABLE_MAILING_INST = 8;
		static int INVALID_UNACCEPTABLE_INVESTOR_ID = 9;
		static int INVALID_UNACCEPTABLE_INVESTOR_ID_SOURCE = 10;
		static int INVALID_UNACCEPTABLE_DATE_OF_BIRTH = 11;
		static int INVALID_UNACCEPTABLE_INVESTOR_COUNTRY_OF_RESIDENCE = 12;
		static int INVALID_UNACCEPTABLE_NODISTRIBINSTNS = 13;
		static int INVALID_UNACCEPTABLE_DISTRIB_PERCENTAGE = 14;
		static int INVALID_UNACCEPTABLE_DISTRIB_PAYMENT_METHOD = 15;
		static int INVALID_UNACCEPTABLE_CASH_DISTRIB_AGENT_ACCT_NAME = 16;
		static int INVALID_UNACCEPTABLE_CASH_DISTRIB_AGENT_CODE = 17;
		static int INVALID_UNACCEPTABLE_CASH_DISTRIB_AGENT_ACCT_NUM = 18;
		static int OTHER = 99;
	};

	namespace RegistRefID_val {
	};

	namespace RegistDtls_val {
	};

	namespace RegistEmail_val {
	};

	namespace RegistID_val {
	};

	namespace RegistTransType_val {
		static const char NEW[] = "0";
		static const char REPLACE[] = "1";
		static const char CANCEL[] = "2";
	};

	namespace OwnershipType_val {
		static const char JOINT_INVESTORS[] = "J";
		static const char TENANTS_IN_COMMON[] = "T";
		static const char JOINT_TRUSTEES[] = "2";
	};

	namespace ContAmtType_val {
		static int COMMISSION_AMOUNT = 1;
		static int COMMISSION_PERCENT = 2;
		static int INITIAL_CHARGE_AMOUNT = 3;
		static int INITIAL_CHARGE_PERCENT = 4;
		static int DISCOUNT_AMOUNT = 5;
		static int DISCOUNT_PERCENT = 6;
		static int DILUTION_LEVY_AMOUNT = 7;
		static int DILUTION_LEVY_PERCENT = 8;
		static int EXIT_CHARGE_AMOUNT = 9;
	};

	namespace OwnerType_val {
		static int INDIVIDUAL_INVESTOR = 1;
		static int PUBLIC_COMPANY = 2;
		static int PRIVATE_COMPANY = 3;
		static int INDIVIDUAL_TRUSTEE = 4;
		static int COMPANY_TRUSTEE = 5;
		static int PENSION_PLAN = 6;
		static int CUSTODIAN_UNDER_GIFTS_TO_MINORS_ACT = 7;
		static int TRUSTS = 8;
		static int FIDUCIARIES = 9;
		static int NETWORKING_SUB_ACCOUNT = 10;
		static int NON_PROFIT_ORGANIZATION = 11;
		static int CORPORATE_BODY = 12;
		static int NOMINEE = 13;
	};

	namespace PartySubID_val {
	};

	namespace NestedPartyID_val {
	};

	namespace NestedPartyIDSource_val {
	};

	namespace SecondaryClOrdID_val {
	};

	namespace SecondaryExecID_val {
	};

	namespace OrderCapacity_val {
		static const char AGENCY[] = "A";
		static const char PROPRIETARY[] = "G";
		static const char INDIVIDUAL[] = "I";
		static const char PRINCIPAL[] = "P";
		static const char RISKLESS_PRINCIPAL[] = "R";
		static const char AGENT_FOR_OTHER_MEMBER[] = "W";
	};

	namespace MassCancelRequestType_val {
		static const char CANCEL_ORDERS_FOR_A_SECURITY[] = "1";
		static const char CANCEL_ORDERS_FOR_AN_UNDERLYING_SECURITY[] = "2";
		static const char CANCEL_ORDERS_FOR_A_PRODUCT[] = "3";
		static const char CANCEL_ORDERS_FOR_A_CFICODE[] = "4";
		static const char CANCEL_ORDERS_FOR_A_SECURITYTYPE[] = "5";
		static const char CANCEL_ORDERS_FOR_A_TRADING_SESSION[] = "6";
		static const char CANCEL_ALL_ORDERS[] = "7";
	};

	namespace MassCancelResponse_val {
		static const char CANCEL_REQUEST_REJECTED[] = "0";
		static const char CANCEL_ORDERS_FOR_A_SECURITY[] = "1";
		static const char CANCEL_ORDERS_FOR_AN_UNDERLYING_SECURITY[] = "2";
		static const char CANCEL_ORDERS_FOR_A_PRODUCT[] = "3";
		static const char CANCEL_ORDERS_FOR_A_CFICODE[] = "4";
		static const char CANCEL_ORDERS_FOR_A_SECURITYTYPE[] = "5";
		static const char CANCEL_ORDERS_FOR_A_TRADING_SESSION[] = "6";
		static const char CANCEL_ALL_ORDERS[] = "7";
	};

	namespace MassCancelRejectReason_val {
		static const char MASS_CANCEL_NOT_SUPPORTED[] = "0";
		static const char INVALID_OR_UNKNOWN_SECURITY[] = "1";
		static const char INVALID_OR_UNKNOWN_UNDERLYING[] = "2";
		static const char INVALID_OR_UNKNOWN_PRODUCT[] = "3";
		static const char INVALID_OR_UNKNOWN_CFICODE[] = "4";
		static const char INVALID_OR_UNKNOWN_SECURITY_TYPE[] = "5";
		static const char INVALID_OR_UNKNOWN_TRADING_SESSION[] = "6";
		static const char OTHER[] = "99";
	};

	namespace TotalAffectedOrders_val {
	};

	namespace NoAffectedOrders_val {
	};

	namespace AffectedOrderID_val {
	};

	namespace AffectedSecondaryOrderID_val {
	};

	namespace QuoteType_val {
		static int INDICATIVE = 0;
		static int TRADEABLE = 1;
		static int RESTRICTED_TRADEABLE = 2;
		static int COUNTER = 3;
	};

	namespace NestedPartyRole_val {
	};

	namespace InstrRegistry_val {
	};

	namespace CashMargin_val {
		static const char CASH[] = "1";
		static const char MARGIN_OPEN[] = "2";
		static const char MARGIN_CLOSE[] = "3";
	};

	namespace NestedPartySubID_val {
	};

	namespace CrossID_val {
	};

	namespace CrossType_val {
		static int CROSS_TRADE_WHICH_IS_EXECUTED_COMPLETELY_OR_NOT = 1;
		static int CROSS_TRADE_WHICH_IS_EXECUTED_PARTIALLY_AND_THE_REST_IS_CANCELLED = 2;
		static int CROSS_TRADE_WHICH_IS_PARTIALLY_EXECUTED_WITH_THE_UNFILLED_PORTIONS_REMAINING_ACTIVE = 3;
		static int CROSS_TRADE_IS_EXECUTED_WITH_EXISTING_ORDERS_WITH_THE_SAME_PRICE = 4;
	};

	namespace CrossPrioritization_val {
		static int NONE = 0;
		static int BUY_SIDE_IS_PRIORITIZED = 1;
		static int SELL_SIDE_IS_PRIORITIZED = 2;
	};

	namespace OrigCrossID_val {
	};

	namespace Username_val {
	};

	namespace Password_val {
	};

	namespace TotNoSecurityTypes_val {
	};

	namespace SecurityListRequestType_val {
		static int SYMBOL = 0;
		static int SECURITYTYPE_AND_OR_CFICODE = 1;
		static int PRODUCT = 2;
		static int TRADINGSESSIONID = 3;
		static int ALL_SECURITIES = 4;
	};

	namespace SecurityRequestResult_val {
		static int VALID_REQUEST = 0;
		static int INVALID_OR_UNSUPPORTED_REQUEST = 1;
		static int NO_INSTRUMENTS_FOUND_THAT_MATCH_SELECTION_CRITERIA = 2;
		static int NOT_AUTHORIZED_TO_RETRIEVE_INSTRUMENT_DATA = 3;
		static int INSTRUMENT_DATA_TEMPORARILY_UNAVAILABLE = 4;
		static int REQUEST_FOR_INSTRUMENT_DATA_NOT_SUPPORTED = 5;
	};

	namespace MultiLegRptTypeReq_val {
		static int REPORT_BY_MULITLEG_SECURITY_ONLY = 0;
		static int REPORT_BY_MULTILEG_SECURITY_AND_BY_INSTRUMENT_LEGS_BELONGING_TO_THE_MULTILEG_SECURITY = 1;
		static int REPORT_BY_INSTRUMENT_LEGS_BELONGING_TO_THE_MULTILEG_SECURITY_ONLY = 2;
	};

	namespace LegPositionEffect_val {
	};

	namespace LegCoveredOrUncovered_val {
	};

	namespace TradSesStatusRejReason_val {
		static int UNKNOWN_OR_INVALID_TRADINGSESSIONID = 1;
	};

	namespace TradeRequestID_val {
	};

	namespace TradeRequestType_val {
		static int ALL_TRADES = 0;
		static int MATCHED_TRADES_MATCHING_CRITERIA_PROVIDED_ON_REQUEST = 1;
		static int UNMATCHED_TRADES_THAT_MATCH_CRITERIA = 2;
		static int UNREPORTED_TRADES_THAT_MATCH_CRITERIA = 3;
		static int ADVISORIES_THAT_MATCH_CRITERIA = 4;
	};

	namespace TradeReportID_val {
	};

	namespace TradeReportRefID_val {
	};

	namespace MatchStatus_val {
		static const char COMPARED_MATCHED_OR_AFFIRMED[] = "0";
		static const char UNCOMPARED_UNMATCHED_OR_UNAFFIRMED[] = "1";
		static const char ADVISORY_OR_ALERT[] = "2";
	};

	namespace MatchType_val {
	};

	namespace NoClearingInstructions_val {
	};

	namespace ClearingInstruction_val {
		static int PROCESS_NORMALLY = 0;
		static int EXCLUDE_FROM_ALL_NETTING = 1;
		static int BILATERAL_NETTING_ONLY = 2;
		static int EX_CLEARING = 3;
		static int SPECIAL_TRADE = 4;
		static int MULTILATERAL_NETTING = 5;
		static int CLEAR_AGAINST_CENTRAL_COUNTERPARTY = 6;
		static int EXCLUDE_FROM_CENTRAL_COUNTERPARTY = 7;
		static int MANUAL_MODE = 8;
		static int AUTOMATIC_POSTING_MODE = 9;
		static int AUTOMATIC_GIVE_UP_MODE = 10;
		static int QUALIFIED_SERVICE_REPRESENTATIVE = 11;
		static int CUSTOMER_TRADE = 12;
		static int SELF_CLEARING = 13;
	};

	namespace TradeInputSource_val {
	};

	namespace TradeInputDevice_val {
	};

	namespace NoDates_val {
	};

	namespace AccountType_val {
		static int ACCOUNT_IS_CARRIED_ON_CUSTOMER_SIDE_OF_BOOKS = 1;
		static int ACCOUNT_IS_CARRIED_ON_NON_CUSTOMER_SIDE_OF_BOOKS = 2;
		static int HOUSE_TRADER = 3;
		static int FLOOR_TRADER = 4;
		static int ACCOUNT_IS_CARRIED_ON_NON_CUSTOMER_SIDE_OF_BOOKS_AND_IS_CROSS_MARGINED = 6;
		static int ACCOUNT_IS_HOUSE_TRADER_AND_IS_CROSS_MARGINED = 7;
		static int JOINT_BACKOFFICE_ACCOUNT = 8;
	};

	namespace CustOrderCapacity_val {
		static int MEMBER_TRADING_FOR_THEIR_OWN_ACCOUNT = 1;
		static int CLEARING_FIRM_TRADING_FOR_ITS_PROPRIETARY_ACCOUNT = 2;
		static int MEMBER_TRADING_FOR_ANOTHER_MEMBER = 3;
		static int ALL_OTHER = 4;
	};

	namespace ClOrdLinkID_val {
	};

	namespace MassStatusReqID_val {
	};

	namespace MassStatusReqType_val {
		static int STATUS_FOR_ORDERS_FOR_A_SECURITY = 1;
		static int STATUS_FOR_ORDERS_FOR_AN_UNDERLYING_SECURITY = 2;
		static int STATUS_FOR_ORDERS_FOR_A_PRODUCT = 3;
		static int STATUS_FOR_ORDERS_FOR_A_CFICODE = 4;
		static int STATUS_FOR_ORDERS_FOR_A_SECURITYTYPE = 5;
		static int STATUS_FOR_ORDERS_FOR_A_TRADING_SESSION = 6;
		static int STATUS_FOR_ALL_ORDERS = 7;
		static int STATUS_FOR_ORDERS_FOR_A_PARTYID = 8;
	};

	namespace LegSettlType_val {
	};

	namespace DayBookingInst_val {
		static const char CAN_TRIGGER_BOOKING_WITHOUT_REFERENCE_TO_THE_ORDER_INITIATOR[] = "0";
		static const char SPEAK_WITH_ORDER_INITIATOR_BEFORE_BOOKING[] = "1";
		static const char ACCUMULATE[] = "2";
	};

	namespace BookingUnit_val {
		static const char EACH_PARTIAL_EXECUTION_IS_A_BOOKABLE_UNIT[] = "0";
		static const char AGGREGATE_PARTIAL_EXECUTIONS_ON_THIS_ORDER_AND_BOOK_ONE_TRADE_PER_ORDER[] = "1";
		static const char AGGREGATE_EXECUTIONS_FOR_THIS_SYMBOL_SIDE_AND_SETTLEMENT_DATE[] = "2";
	};

	namespace PreallocMethod_val {
		static const char PRO_RATA[] = "0";
		static const char DO_NOT_PRO_RATA[] = "1";
	};

	namespace UnderlyingStateOrProvinceOfIssue_val {
	};

	namespace UnderlyingLocaleOfIssue_val {
	};

	namespace UnderlyingInstrRegistry_val {
	};

	namespace LegStateOrProvinceOfIssue_val {
	};

	namespace LegLocaleOfIssue_val {
	};

	namespace LegInstrRegistry_val {
	};

	namespace LegSymbol_val {
	};

	namespace LegSymbolSfx_val {
	};

	namespace LegSecurityID_val {
	};

	namespace LegSecurityIDSource_val {
	};

	namespace NoLegSecurityAltID_val {
	};

	namespace LegSecurityAltID_val {
	};

	namespace LegSecurityAltIDSource_val {
	};

	namespace LegProduct_val {
	};

	namespace LegCFICode_val {
	};

	namespace LegSecurityType_val {
	};

	namespace LegOptAttribute_val {
	};

	namespace LegIssuer_val {
	};

	namespace LegSecurityDesc_val {
	};

	namespace LegSide_val {
	};

	namespace TradingSessionSubID_val {
	};

	namespace AllocType_val {
		static int CALCULATED = 1;
		static int PRELIMINARY = 2;
		static int READY_TO_BOOK_SINGLE_ORDER = 5;
		static int WAREHOUSE_INSTRUCTION = 7;
		static int REQUEST_TO_INTERMEDIARY = 8;
	};

	namespace HopCompID_val {
	};

	namespace ClearingFeeIndicator_val {
		static const char CBOE_MEMBER[] = "B";
		static const char NON_MEMBER_AND_CUSTOMER[] = "C";
		static const char EQUITY_MEMBER_AND_CLEARING_MEMBER[] = "E";
		static const char FULL_AND_ASSOCIATE_MEMBER_TRADING_FOR_OWN_ACCOUNT_AND_AS_FLOOR_BROKERS[] = "F";
		static const char FIRMS_106H_AND_106J[] = "H";
		static const char GIM_IDEM_AND_COM_MEMBERSHIP_INTEREST_HOLDERS[] = "I";
		static const char LESSEE_AND_106F_EMPLOYEES[] = "L";
		static const char ALL_OTHER_OWNERSHIP_TYPES[] = "M";
	};

	namespace PriorityIndicator_val {
		static int PRIORITY_UNCHANGED = 0;
		static int LOST_PRIORITY_AS_RESULT_OF_ORDER_CHANGE = 1;
	};

	namespace RFQReqID_val {
	};

	namespace QuoteStatusReqID_val {
	};

	namespace LegRefID_val {
	};

	namespace ContraLegRefID_val {
	};

	namespace QuoteRequestRejectReason_val {
		static int UNKNOWN_SYMBOL = 1;
		static int EXCHANGE_CLOSED = 2;
		static int QUOTE_REQUEST_EXCEEDS_LIMIT = 3;
		static int TOO_LATE_TO_ENTER = 4;
		static int INVALID_PRICE = 5;
		static int NOT_AUTHORIZED_TO_REQUEST_QUOTE = 6;
		static int NO_MATCH_FOR_INQUIRY = 7;
		static int NO_MARKET_FOR_INSTRUMENT = 8;
		static int NO_INVENTORY = 9;
		static int PASS = 10;
		static int OTHER = 99;
	};

	namespace SideComplianceID_val {
	};

	namespace AcctIDSource_val {
		static int BIC = 1;
		static int SID_CODE = 2;
		static int TFM = 3;
		static int OMGEO = 4;
		static int DTCC_CODE = 5;
		static int OTHER = 99;
	};

	namespace AllocAcctIDSource_val {
	};

	namespace BenchmarkPriceType_val {
	};

	namespace ConfirmID_val {
	};

	namespace ConfirmStatus_val {
		static int RECEIVED = 1;
		static int MISMATCHED_ACCOUNT = 2;
		static int MISSING_SETTLEMENT_INSTRUCTIONS = 3;
		static int CONFIRMED = 4;
		static int REQUEST_REJECTED = 5;
	};

	namespace ConfirmTransType_val {
		static int NEW = 0;
		static int REPLACE = 1;
		static int CANCEL = 2;
	};

	namespace DeliveryForm_val {
		static int BOOKENTRY = 1;
		static int BEARER = 2;
	};

	namespace LegAllocAccount_val {
	};

	namespace LegIndividualAllocID_val {
	};

	namespace LegAllocAcctIDSource_val {
	};

	namespace LegBenchmarkCurveName_val {
	};

	namespace LegBenchmarkCurvePoint_val {
	};

	namespace LegBenchmarkPriceType_val {
	};

	namespace LegIOIQty_val {
	};

	namespace LegPriceType_val {
	};

	namespace LegStipulationType_val {
	};

	namespace LegStipulationValue_val {
	};

	namespace LegSwapType_val {
		static int PAR_FOR_PAR = 1;
		static int MODIFIED_DURATION = 2;
		static int RISK = 4;
		static int PROCEEDS = 5;
	};

	namespace Pool_val {
	};

	namespace QuotePriceType_val {
		static int PERCENT = 1;
		static int PER_SHARE = 2;
		static int FIXED_AMOUNT = 3;
		static int DISCOUNT = 4;
		static int PREMIUM = 5;
		static int BASIS_POINTS_RELATIVE_TO_BENCHMARK = 6;
		static int TED_PRICE = 7;
		static int TED_YIELD = 8;
		static int YIELD_SPREAD = 9;
		static int YIELD = 10;
	};

	namespace QuoteRespID_val {
	};

	namespace QuoteRespType_val {
		static int HIT_LIFT = 1;
		static int COUNTER = 2;
		static int EXPIRED = 3;
		static int COVER = 4;
		static int DONE_AWAY = 5;
		static int PASS = 6;
	};

	namespace QuoteQualifier_val {
	};

	namespace YieldRedemptionPriceType_val {
	};

	namespace BenchmarkSecurityID_val {
	};

	namespace PosType_val {
		static const char TRANSACTION_QUANTITY[] = "TQ";
		static const char INTRA_SPREAD_QTY[] = "IAS";
		static const char INTER_SPREAD_QTY[] = "IES";
		static const char END_OF_DAY_QTY[] = "FIN";
		static const char START_OF_DAY_QTY[] = "SOD";
		static const char OPTION_EXERCISE_QTY[] = "EX";
		static const char OPTION_ASSIGNMENT[] = "AS";
		static const char TRANSACTION_FROM_EXERCISE[] = "TX";
		static const char TRANSACTION_FROM_ASSIGNMENT[] = "TA";
		static const char PIT_TRADE_QTY[] = "PIT";
		static const char TRANSFER_TRADE_QTY[] = "TRF";
		static const char ELECTRONIC_TRADE_QTY[] = "ETR";
		static const char ALLOCATION_TRADE_QTY[] = "ALC";
		static const char ADJUSTMENT_QTY[] = "PA";
		static const char AS_OF_TRADE_QTY[] = "ASF";
		static const char DELIVERY_QTY[] = "DLV";
		static const char TOTAL_TRANSACTION_QTY[] = "TOT";
		static const char CROSS_MARGIN_QTY[] = "XM";
		static const char INTEGRAL_SPLIT[] = "SPL";
	};

	namespace PosQtyStatus_val {
		static int SUBMITTED = 0;
		static int ACCEPTED = 1;
		static int REJECTED = 2;
	};

	namespace PosAmtType_val {
		static const char FINAL_MARK_TO_MARKET_AMOUNT[] = "FMTM";
		static const char INCREMENTAL_MARK_TO_MARKET_AMOUNT[] = "IMTM";
		static const char TRADE_VARIATION_AMOUNT[] = "TVAR";
		static const char START_OF_DAY_MARK_TO_MARKET_AMOUNT[] = "SMTM";
		static const char PREMIUM_AMOUNT[] = "PREM";
		static const char CASH_RESIDUAL_AMOUNT[] = "CRES";
		static const char CASH_AMOUNT[] = "CASH";
		static const char VALUE_ADJUSTED_AMOUNT[] = "VADJ";
	};

	namespace PosTransType_val {
		static int EXERCISE = 1;
		static int DO_NOT_EXERCISE = 2;
		static int POSITION_ADJUSTMENT = 3;
		static int POSITION_CHANGE_SUBMISSION_MARGIN_DISPOSITION = 4;
		static int PLEDGE = 5;
	};

	namespace PosReqID_val {
	};

	namespace PosMaintAction_val {
		static int NEW = 1;
		static int REPLACE = 2;
		static int CANCEL = 3;
	};

	namespace OrigPosReqRefID_val {
	};

	namespace PosMaintRptRefID_val {
	};

	namespace SettlSessID_val {
	};

	namespace SettlSessSubID_val {
	};

	namespace AdjustmentType_val {
		static int PROCESS_REQUEST_AS_MARGIN_DISPOSITION = 0;
		static int DELTA_PLUS = 1;
		static int DELTA_MINUS = 2;
		static int FINAL = 3;
	};

	namespace PosMaintRptID_val {
	};

	namespace PosMaintStatus_val {
		static int ACCEPTED = 0;
		static int ACCEPTED_WITH_WARNINGS = 1;
		static int REJECTED = 2;
		static int COMPLETED = 3;
		static int COMPLETED_WITH_WARNINGS = 4;
	};

	namespace PosMaintResult_val {
		static int SUCCESSFUL_COMPLETION_NO_WARNINGS_OR_ERRORS = 0;
		static int REJECTED = 1;
		static int OTHER = 99;
	};

	namespace PosReqType_val {
		static int POSITIONS = 0;
		static int TRADES = 1;
		static int EXERCISES = 2;
		static int ASSIGNMENTS = 3;
	};

	namespace ResponseTransportType_val {
		static int INBAND = 0;
		static int OUT_OF_BAND = 1;
	};

	namespace ResponseDestination_val {
	};

	namespace TotalNumPosReports_val {
	};

	namespace PosReqResult_val {
		static int VALID_REQUEST = 0;
		static int INVALID_OR_UNSUPPORTED_REQUEST = 1;
		static int NO_POSITIONS_FOUND_THAT_MATCH_CRITERIA = 2;
		static int NOT_AUTHORIZED_TO_REQUEST_POSITIONS = 3;
		static int REQUEST_FOR_POSITION_NOT_SUPPORTED = 4;
		static int OTHER = 99;
	};

	namespace PosReqStatus_val {
		static int COMPLETED = 0;
		static int COMPLETED_WITH_WARNINGS = 1;
		static int REJECTED = 2;
	};

	namespace SettlPriceType_val {
		static int FINAL = 1;
		static int THEORETICAL = 2;
	};

	namespace UnderlyingSettlPriceType_val {
	};

	namespace LegPool_val {
	};

	namespace AssignmentMethod_val {
		static const char RANDOM[] = "R";
		static const char PRORATA[] = "P";
	};

	namespace ExerciseMethod_val {
		static const char AUTOMATIC[] = "A";
		static const char MANUAL[] = "M";
	};

	namespace TotNumTradeReports_val {
	};

	namespace TradeRequestResult_val {
		static int SUCCESSFUL = 0;
		static int INVALID_OR_UNKNOWN_INSTRUMENT = 1;
		static int INVALID_TYPE_OF_TRADE_REQUESTED = 2;
		static int INVALID_PARTIES = 3;
		static int INVALID_TRANSPORT_TYPE_REQUESTED = 4;
		static int INVALID_DESTINATION_REQUESTED = 5;
		static int TRADEREQUESTTYPE_NOT_SUPPORTED = 8;
		static int UNAUTHORIZED_FOR_TRADE_CAPTURE_REPORT_REQUEST = 9;
		static int OTHER = 99;
	};

	namespace TradeRequestStatus_val {
		static int ACCEPTED = 0;
		static int COMPLETED = 1;
		static int REJECTED = 2;
	};

	namespace TradeReportRejectReason_val {
		static int SUCCESSFUL = 0;
		static int INVALID_PARTY_INFORMATION = 1;
		static int UNKNOWN_INSTRUMENT = 2;
		static int UNAUTHORIZED_TO_REPORT_TRADES = 3;
		static int INVALID_TRADE_TYPE = 4;
		static int OTHER = 99;
	};

	namespace SideMultiLegReportingType_val {
		static int SINGLE_SECURITY = 1;
		static int INDIVIDUAL_LEG_OF_A_MULTI_LEG_SECURITY = 2;
		static int MULTI_LEG_SECURITY = 3;
	};

	namespace AllocReportID_val {
	};

	namespace Nested2PartyID_val {
	};

	namespace Nested2PartyIDSource_val {
	};

	namespace Nested2PartyRole_val {
	};

	namespace Nested2PartySubID_val {
	};

	namespace BenchmarkSecurityIDSource_val {
		static const char CUSIP[] = "1";
		static const char SEDOL[] = "2";
		static const char QUIK[] = "3";
		static const char ISIN_NUMBER[] = "4";
		static const char RIC_CODE[] = "5";
		static const char ISO_CURRENCY_CODE[] = "6";
		static const char ISO_COUNTRY_CODE[] = "7";
		static const char EXCHANGE_SYMBOL[] = "8";
		static const char CONSOLIDATED_TAPE_ASSOCIATION[] = "9";
		static const char BLOOMBERG_SYMBOL[] = "A";
		static const char WERTPAPIER[] = "B";
		static const char DUTCH[] = "C";
		static const char VALOREN[] = "D";
		static const char SICOVAM[] = "E";
		static const char BELGIAN[] = "F";
		static const char COMMON[] = "G";
		static const char CLEARING_HOUSE_CLEARING_ORGANIZATION[] = "H";
		static const char ISDA_FPML_PRODUCT_SPECIFICATION[] = "I";
		static const char OPTIONS_PRICE_REPORTING_AUTHORITY[] = "J";
	};

	namespace SecuritySubType_val {
	};

	namespace UnderlyingSecuritySubType_val {
	};

	namespace LegSecuritySubType_val {
	};

	namespace TrdRegTimestampType_val {
		static int EXECUTION_TIME = 1;
		static int TIME_IN = 2;
		static int TIME_OUT = 3;
		static int BROKER_RECEIPT = 4;
		static int BROKER_EXECUTION = 5;
	};

	namespace TrdRegTimestampOrigin_val {
	};

	namespace ConfirmRefID_val {
	};

	namespace ConfirmType_val {
		static int STATUS = 1;
		static int CONFIRMATION = 2;
		static int CONFIRMATION_REQUEST_REJECTED = 3;
	};

	namespace ConfirmRejReason_val {
		static int MISMATCHED_ACCOUNT = 1;
		static int MISSING_SETTLEMENT_INSTRUCTIONS = 2;
		static int OTHER = 99;
	};

	namespace BookingType_val {
		static int REGULAR_BOOKING = 0;
		static int CFD = 1;
		static int TOTAL_RETURN_SWAP = 2;
	};

	namespace IndividualAllocRejCode_val {
	};

	namespace SettlInstMsgID_val {
	};

	namespace AllocSettlInstType_val {
		static int USE_DEFAULT_INSTRUCTIONS = 0;
		static int DERIVE_FROM_PARAMETERS_PROVIDED = 1;
		static int FULL_DETAILS_PROVIDED = 2;
		static int SSI_DB_IDS_PROVIDED = 3;
		static int PHONE_FOR_INSTRUCTIONS = 4;
	};

	namespace SettlPartyID_val {
	};

	namespace SettlPartyIDSource_val {
	};

	namespace SettlPartyRole_val {
	};

	namespace SettlPartySubID_val {
	};

	namespace SettlPartySubIDType_val {
	};

	namespace DlvyInstType_val {
		static const char SECURITIES[] = "S";
		static const char CASH[] = "C";
	};

	namespace TerminationType_val {
		static int OVERNIGHT = 1;
		static int TERM = 2;
		static int FLEXIBLE = 3;
		static int OPEN = 4;
	};

	namespace OrdStatusReqID_val {
	};

	namespace SettlInstReqID_val {
	};

	namespace SettlInstReqRejCode_val {
		static int UNABLE_TO_PROCESS_REQUEST = 0;
		static int UNKNOWN_ACCOUNT = 1;
		static int NO_MATCHING_SETTLEMENT_INSTRUCTIONS_FOUND = 2;
		static int OTHER = 99;
	};

	namespace SecondaryAllocID_val {
	};

	namespace AllocReportType_val {
		static int SELLSIDE_CALCULATED_USING_PRELIMINARY = 3;
		static int SELLSIDE_CALCULATED_WITHOUT_PRELIMINARY = 4;
		static int WAREHOUSE_RECAP = 5;
		static int REQUEST_TO_INTERMEDIARY = 8;
	};

	namespace AllocReportRefID_val {
	};

	namespace AllocCancReplaceReason_val {
		static int ORIGINAL_DETAILS_INCOMPLETE_INCORRECT = 1;
		static int CHANGE_IN_UNDERLYING_ORDER_DETAILS = 2;
	};

	namespace AllocAccountType_val {
		static int ACCOUNT_IS_CARRIED_ON_CUSTOMER_SIDE_OF_BOOKS = 1;
		static int ACCOUNT_IS_CARRIED_ON_NON_CUSTOMER_SIDE_OF_BOOKS = 2;
		static int HOUSE_TRADER = 3;
		static int FLOOR_TRADER = 4;
		static int ACCOUNT_IS_CARRIED_ON_NON_CUSTOMER_SIDE_OF_BOOKS_AND_IS_CROSS_MARGINED = 6;
		static int ACCOUNT_IS_HOUSE_TRADER_AND_IS_CROSS_MARGINED = 7;
		static int JOINT_BACKOFFICE_ACCOUNT = 8;
	};

	namespace PartySubIDType_val {
	};

	namespace NestedPartySubIDType_val {
	};

	namespace Nested2PartySubIDType_val {
	};

	namespace AllocIntermedReqType_val {
		static int PENDING_ACCEPT = 1;
		static int PENDING_RELEASE = 2;
		static int PENDING_REVERSAL = 3;
		static int ACCEPT = 4;
		static int BLOCK_LEVEL_REJECT = 5;
		static int ACCOUNT_LEVEL_REJECT = 6;
	};

	namespace ApplQueueMax_val {
	};

	namespace ApplQueueDepth_val {
	};

	namespace ApplQueueResolution_val {
		static int NO_ACTION_TAKEN = 0;
		static int QUEUE_FLUSHED = 1;
		static int OVERLAY_LAST = 2;
		static int END_SESSION = 3;
	};

	namespace ApplQueueAction_val {
		static int NO_ACTION_TAKEN = 0;
		static int QUEUE_FLUSHED = 1;
		static int OVERLAY_LAST = 2;
		static int END_SESSION = 3;
	};

	namespace AltMDSourceID_val {
	};

	namespace SecondaryTradeReportID_val {
	};

	namespace AvgPxIndicator_val {
		static int NO_AVERAGE_PRICING = 0;
		static int TRADE_IS_PART_OF_AN_AVERAGE_PRICE_GROUP_IDENTIFIED_BY_THE_TRADELINKID = 1;
		static int LAST_TRADE_IN_THE_AVERAGE_PRICE_GROUP_IDENTIFIED_BY_THE_TRADELINKID = 2;
	};

	namespace TradeLinkID_val {
	};

	namespace OrderInputDevice_val {
	};

	namespace UnderlyingTradingSessionID_val {
	};

	namespace UnderlyingTradingSessionSubID_val {
	};

	namespace TradeLegRefID_val {
	};

	namespace ExchangeRule_val {
	};

	namespace TradeAllocIndicator_val {
		static int ALLOCATION_NOT_REQUIRED = 0;
		static int ALLOCATION_REQUIRED = 1;
		static int USE_ALLOCATION_PROVIDED_WITH_THE_TRADE = 2;
	};

	namespace ExpirationCycle_val {
		static int EXPIRE_ON_TRADING_SESSION_CLOSE = 0;
		static int EXPIRE_ON_TRADING_SESSION_OPEN = 1;
	};

	namespace TrdType_val {
		static int REGULAR_TRADE = 0;
		static int BLOCK_TRADE = 1;
		static int EFP = 2;
		static int TRANSFER = 3;
		static int LATE_TRADE = 4;
		static int T_TRADE = 5;
		static int WEIGHTED_AVERAGE_PRICE_TRADE = 6;
		static int BUNCHED_TRADE = 7;
		static int LATE_BUNCHED_TRADE = 8;
		static int PRIOR_REFERENCE_PRICE_TRADE = 9;
		static int AFTER_HOURS_TRADE = 10;
	};

	namespace TrdSubType_val {
	};

	namespace TransferReason_val {
	};

	namespace AsgnReqID_val {
	};

	namespace TotNumAssignmentReports_val {
	};

	namespace AsgnRptID_val {
	};

	namespace PegMoveType_val {
		static int FLOATING = 0;
		static int FIXED = 1;
	};

	namespace PegOffsetType_val {
		static int PRICE = 0;
		static int BASIS_POINTS = 1;
		static int TICKS = 2;
		static int PRICE_TIER_LEVEL = 3;
	};

	namespace PegLimitType_val {
		static int OR_BETTER = 0;
		static int STRICT = 1;
		static int OR_WORSE = 2;
	};

	namespace PegRoundDirection_val {
		static int MORE_AGGRESSIVE = 1;
		static int MORE_PASSIVE = 2;
	};

	namespace PegScope_val {
		static int LOCAL = 1;
		static int NATIONAL = 2;
		static int GLOBAL = 3;
		static int NATIONAL_EXCLUDING_LOCAL = 4;
	};

	namespace DiscretionMoveType_val {
		static int FLOATING = 0;
		static int FIXED = 1;
	};

	namespace DiscretionOffsetType_val {
		static int PRICE = 0;
		static int BASIS_POINTS = 1;
		static int TICKS = 2;
		static int PRICE_TIER_LEVEL = 3;
	};

	namespace DiscretionLimitType_val {
		static int OR_BETTER = 0;
		static int STRICT = 1;
		static int OR_WORSE = 2;
	};

	namespace DiscretionRoundDirection_val {
		static int MORE_AGGRESSIVE = 1;
		static int MORE_PASSIVE = 2;
	};

	namespace DiscretionScope_val {
		static int LOCAL = 1;
		static int NATIONAL = 2;
		static int GLOBAL = 3;
		static int NATIONAL_EXCLUDING_LOCAL = 4;
	};

	namespace TargetStrategy_val {
	};

	namespace TargetStrategyParameters_val {
	};

	namespace LastLiquidityInd_val {
		static int ADDED_LIQUIDITY = 1;
		static int REMOVED_LIQUIDITY = 2;
		static int LIQUIDITY_ROUTED_OUT = 3;
	};

	namespace ShortSaleReason_val {
		static int DEALER_SOLD_SHORT = 0;
		static int DEALER_SOLD_SHORT_EXEMPT = 1;
		static int SELLING_CUSTOMER_SOLD_SHORT = 2;
		static int SELLING_CUSTOMER_SOLD_SHORT_EXEMPT = 3;
		static int QUALIFED_SERVICE_REPRESENTATIVE_OR_AUTOMATIC_GIVEUP_CONTRA_SIDE_SOLD_SHORT = 4;
		static int QSR_OR_AGU_CONTRA_SIDE_SOLD_SHORT_EXEMPT = 5;
	};

	namespace QtyType_val {
		static int UNITS = 0;
		static int CONTRACTS = 1;
	};

	namespace SecondaryTrdType_val {
	};

	namespace TradeReportType_val {
		static int SUBMIT = 0;
		static int ALLEGED = 1;
		static int ACCEPT = 2;
		static int DECLINE = 3;
		static int ADDENDUM = 4;
		static int NO_WAS = 5;
		static int TRADE_REPORT_CANCEL = 6;
		static int LOCKED_IN_TRADE_BREAK = 7;
	};

	namespace AllocNoOrdersType_val {
		static int NOT_SPECIFIED = 0;
		static int EXPLICIT_LIST_PROVIDED = 1;
	};

	namespace ConfirmReqID_val {
	};

	namespace EventType_val {
		static int PUT = 1;
		static int CALL = 2;
		static int TENDER = 3;
		static int SINKING_FUND_CALL = 4;
		static int OTHER = 99;
	};

	namespace EventText_val {
	};

	namespace InstrAttribType_val {
		static int FLAT = 1;
		static int ZERO_COUPON = 2;
		static int INTEREST_BEARING = 3;
		static int NO_PERIODIC_PAYMENTS = 4;
		static int VARIABLE_RATE = 5;
		static int LESS_FEE_FOR_PUT = 6;
		static int STEPPED_COUPON = 7;
		static int COUPON_PERIOD = 8;
		static int WHEN_AND_IF_ISSUED = 9;
		static int ORIGINAL_ISSUE_DISCOUNT = 10;
		static int CALLABLE_PUTTABLE = 11;
		static int ESCROWED_TO_MATURITY = 12;
		static int ESCROWED_TO_REDEMPTION_DATE = 13;
		static int PRE_REFUNDED = 14;
		static int IN_DEFAULT = 15;
		static int UNRATED = 16;
		static int TAXABLE = 17;
		static int INDEXED = 18;
		static int SUBJECT_TO_ALTERNATIVE_MINIMUM_TAX = 19;
		static int ORIGINAL_ISSUE_DISCOUNT_PRICE = 20;
		static int CALLABLE_BELOW_MATURITY_VALUE = 21;
		static int CALLABLE_WITHOUT_NOTICE_BY_MAIL_TO_HOLDER_UNLESS_REGISTERED = 22;
		static int TEXT = 99;
	};

	namespace InstrAttribValue_val {
	};

	namespace CPProgram_val {
	};

	namespace CPRegType_val {
	};

	namespace UnderlyingCPProgram_val {
	};

	namespace UnderlyingCPRegType_val {
	};

	namespace TrdMatchID_val {
	};

	namespace SecondaryTradeReportRefID_val {
	};

	namespace UnderlyingStipType_val {
	};

	namespace UnderlyingStipValue_val {
	};

	namespace MiscFeeBasis_val {
		static int ABSOLUTE = 0;
		static int PER_UNIT = 1;
		static int PERCENTAGE = 2;
	};

	namespace TotNoAllocs_val {
	};

	namespace CollReqID_val {
	};

	namespace CollAsgnReason_val {
		static int INITIAL = 0;
		static int SCHEDULED = 1;
		static int TIME_WARNING = 2;
		static int MARGIN_DEFICIENCY = 3;
		static int MARGIN_EXCESS = 4;
		static int FORWARD_COLLATERAL_DEMAND = 5;
		static int EVENT_OF_DEFAULT = 6;
		static int ADVERSE_TAX_EVENT = 7;
	};

	namespace CollInquiryQualifier_val {
		static int TRADEDATE = 0;
		static int GC_INSTRUMENT = 1;
		static int COLLATERALINSTRUMENT = 2;
		static int SUBSTITUTION_ELIGIBLE = 3;
		static int NOT_ASSIGNED = 4;
		static int PARTIALLY_ASSIGNED = 5;
		static int FULLY_ASSIGNED = 6;
		static int OUTSTANDING_TRADES = 7;
	};

	namespace CollAsgnID_val {
	};

	namespace CollAsgnTransType_val {
		static int NEW = 0;
		static int REPLACE = 1;
		static int CANCEL = 2;
		static int RELEASE = 3;
		static int REVERSE = 4;
	};

	namespace CollRespID_val {
	};

	namespace CollAsgnRespType_val {
		static int RECEIVED = 0;
		static int ACCEPTED = 1;
		static int DECLINED = 2;
		static int REJECTED = 3;
	};

	namespace CollAsgnRejectReason_val {
		static int UNKNOWN_DEAL = 0;
		static int UNKNOWN_OR_INVALID_INSTRUMENT = 1;
		static int UNAUTHORIZED_TRANSACTION = 2;
		static int INSUFFICIENT_COLLATERAL = 3;
		static int INVALID_TYPE_OF_COLLATERAL = 4;
		static int EXCESSIVE_SUBSTITUTION = 5;
		static int OTHER = 99;
	};

	namespace CollAsgnRefID_val {
	};

	namespace CollRptID_val {
	};

	namespace CollInquiryID_val {
	};

	namespace CollStatus_val {
		static int UNASSIGNED = 0;
		static int PARTIALLY_ASSIGNED = 1;
		static int ASSIGNMENT_PROPOSED = 2;
		static int ASSIGNED = 3;
		static int CHALLENGED = 4;
	};

	namespace TotNumReports_val {
	};

	namespace AgreementDesc_val {
	};

	namespace AgreementID_val {
	};

	namespace DeliveryType_val {
		static int VERSUS_PAYMENT = 0;
		static int FREE = 1;
		static int TRI_PARTY = 2;
		static int HOLD_IN_CUSTODY = 3;
	};

	namespace UserRequestID_val {
	};

	namespace UserRequestType_val {
		static int LOGONUSER = 1;
		static int LOGOFFUSER = 2;
		static int CHANGEPASSWORDFORUSER = 3;
		static int REQUEST_INDIVIDUAL_USER_STATUS = 4;
	};

	namespace NewPassword_val {
	};

	namespace UserStatus_val {
		static int LOGGED_IN = 1;
		static int NOT_LOGGED_IN = 2;
		static int USER_NOT_RECOGNISED = 3;
		static int PASSWORD_INCORRECT = 4;
		static int PASSWORD_CHANGED = 5;
		static int OTHER = 6;
	};

	namespace UserStatusText_val {
	};

	namespace StatusValue_val {
		static int CONNECTED = 1;
		static int NOT_CONNECTED_DOWN_EXPECTED_UP = 2;
		static int NOT_CONNECTED_DOWN_EXPECTED_DOWN = 3;
		static int IN_PROCESS = 4;
	};

	namespace StatusText_val {
	};

	namespace RefCompID_val {
	};

	namespace RefSubID_val {
	};

	namespace NetworkResponseID_val {
	};

	namespace NetworkRequestID_val {
	};

	namespace LastNetworkResponseID_val {
	};

	namespace NetworkRequestType_val {
		static int SNAPSHOT = 1;
		static int SUBSCRIBE = 2;
		static int STOP_SUBSCRIBING = 4;
		static int LEVEL_OF_DETAIL = 8;
	};

	namespace NetworkStatusResponseType_val {
		static int FULL = 1;
		static int INCREMENTAL_UPDATE = 2;
	};

	namespace TrdRptStatus_val {
		static int ACCEPTED = 0;
		static int REJECTED = 1;
	};

	namespace AffirmStatus_val {
		static int RECEIVED = 1;
		static int CONFIRM_REJECTED = 2;
		static int AFFIRMED = 3;
	};

	namespace TimeBracket_val {
	};

	namespace CollAction_val {
		static int RETAIN = 0;
		static int ADD = 1;
		static int REMOVE = 2;
	};

	namespace CollInquiryStatus_val {
		static int ACCEPTED = 0;
		static int ACCEPTED_WITH_WARNINGS = 1;
		static int COMPLETED = 2;
		static int COMPLETED_WITH_WARNINGS = 3;
		static int REJECTED = 4;
	};

	namespace CollInquiryResult_val {
		static int SUCCESSFUL = 0;
		static int INVALID_OR_UNKNOWN_INSTRUMENT = 1;
		static int INVALID_OR_UNKNOWN_COLLATERAL_TYPE = 2;
		static int INVALID_PARTIES = 3;
		static int INVALID_TRANSPORT_TYPE_REQUESTED = 4;
		static int INVALID_DESTINATION_REQUESTED = 5;
		static int NO_COLLATERAL_FOUND_FOR_THE_TRADE_SPECIFIED = 6;
		static int NO_COLLATERAL_FOUND_FOR_THE_ORDER_SPECIFIED = 7;
		static int COLLATERAL_INQUIRY_TYPE_NOT_SUPPORTED = 8;
		static int UNAUTHORIZED_FOR_COLLATERAL_INQUIRY = 9;
		static int OTHER = 99;
	};

	namespace Nested3PartyID_val {
	};

	namespace Nested3PartyIDSource_val {
	};

	namespace Nested3PartyRole_val {
	};

	namespace Nested3PartySubID_val {
	};

	namespace Nested3PartySubIDType_val {
	};

	namespace MDEntryMakerSide_val {
		static const char BUY[] = "1";
		static const char SELL[] = "2";
	};

};