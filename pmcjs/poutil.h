
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_POUTIL_HPP
#define PMCJS_POUTIL_HPP

#ifdef __cplusplus
extern "C" {
#endif

void *pmc_malloc_nocx(size_t len, const char *debugid);
void pmc_free(void *p, const char *debugid);
void pmc_dump_str(const char *mess, const char *str, size_t slen);

#ifdef __cplusplus
}
#endif

#endif

