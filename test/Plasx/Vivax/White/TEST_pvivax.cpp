#include "PlasX/Vivax/White/pvivax.hpp"
#include "PlasX/udl.hpp"
#include "gtest/gtest.h"
using namespace plasx;
namespace pvibm = plasx::vivax::white;

TEST(pvivax, constructor) {
  auto person = pvibm::PVivax(0.0, pvibm::Status::S, 0.0, 0.0, 0.0, 0.0, 0.0,
                              0.0, 1.0, 0);
  EXPECT_EQ(person.getParasiteImmunity(), 0.0);
  EXPECT_EQ(person.getClinicalImmunity(), 0.0);
  EXPECT_EQ(person.getZeta(), 0.0);
  EXPECT_EQ(person.getNumHypnozoites(), std::size_t(0));

  auto test_zeta = 12.5;
  auto test_rho = 0.2;
  auto test_age_0 = 1.0;
  auto clinical_immunity = 12.0;
  auto parasite_immunity = 10.0;
  auto maternal_clinical = 50.0;
  auto maternal_parasite = 5.9;
  auto person2 = pvibm::PVivax(
      0.0, pvibm::Status::S, parasite_immunity, clinical_immunity,
      maternal_parasite, maternal_clinical, test_zeta, test_rho, test_age_0, 0);
  EXPECT_EQ(person2.getParasiteImmunity(),
            parasite_immunity + maternal_parasite);
  EXPECT_EQ(person2.getClinicalImmunity(),
            clinical_immunity + maternal_clinical);
  EXPECT_EQ(person2.getZeta(), test_zeta);
  EXPECT_EQ(person2.getNumHypnozoites(), std::size_t(0));
}

TEST(pvivax, changeHypnozoites) {
  auto test_zeta = 12.5;
  auto test_rho = 0.2;
  auto test_age_0 = 1.0;
  auto clinical_immunity = 12.0;
  auto parasite_immunity = 10.0;
  auto maternal_clinical = 50.0;
  auto maternal_parasite = 5.9;
  auto person = pvibm::PVivax(
      0.0, pvibm::Status::S, parasite_immunity, clinical_immunity,
      maternal_parasite, maternal_clinical, test_zeta, test_rho, test_age_0, 0);
  EXPECT_EQ(person.getParasiteImmunity(),
            parasite_immunity + maternal_parasite);
  EXPECT_EQ(person.getClinicalImmunity(),
            clinical_immunity + maternal_clinical);
  EXPECT_EQ(person.getZeta(), test_zeta);
  EXPECT_EQ(person.getNumHypnozoites(), std::size_t(0));
  EXPECT_EQ(++person.getNumHypnozoites(), std::size_t(1));
  EXPECT_EQ(++person.getNumHypnozoites(), std::size_t(2));
  EXPECT_EQ(--person.getNumHypnozoites(), std::size_t(1));
  EXPECT_EQ(--person.getNumHypnozoites(), std::size_t(0));
}

TEST(pvivax, Infections) {
  auto test_rho = 0.2;
  auto test_age_0 = 1.0;
  auto test_zeta = 12.5;
  auto clinical_immunity = 12.0;
  auto parasite_immunity = 10.0;
  auto maternal_clinical = 50.0;
  auto maternal_parasite = 5.9;
  auto person = pvibm::PVivax(
      0.0, pvibm::Status::S, parasite_immunity, clinical_immunity,
      maternal_parasite, maternal_clinical, test_zeta, test_rho, test_age_0, 0);

  auto return_type = person.updateInfection(1.0, 0.0);
  EXPECT_EQ(return_type, false);
  return_type = person.updateInfection(100.0, 0.0);
  EXPECT_EQ(return_type, false);

  person.queueInfection(10.0, 1.0, 1.0, 10.0);
  person.queueInfection(15.0, 0.0, 1.0, 10.0);
  EXPECT_EQ(person.getNumHypnozoites(),std::size_t(0));

  return_type = person.updateInfection(20.0, 0.0);
  EXPECT_EQ(return_type, true);
  EXPECT_EQ(person.getNumHypnozoites(), std::size_t(1));
  // Update but dont change
  person.updateImmunity(0.0, 1.0, 1.0, 1.0, 0.0, 1.0_yrs);
  EXPECT_EQ(person.getParasiteImmunity(),
            parasite_immunity + maternal_parasite + 1.0);
  EXPECT_EQ(person.getClinicalImmunity(),
            clinical_immunity + maternal_clinical + 1.0);

  return_type = person.updateInfection(21.0, 0.0);
  EXPECT_EQ(return_type, false);
  person.updateImmunity(0.0, 1.0, 1.0, 1.0, 0.0, 1.0_yrs);
  EXPECT_EQ(person.getParasiteImmunity(),
            parasite_immunity + maternal_parasite + 1.0);
  EXPECT_EQ(person.getClinicalImmunity(),
            clinical_immunity + maternal_clinical + 1.0);

  return_type = person.updateInfection(25.1, 0.0);
  EXPECT_EQ(return_type, true);
  person.updateImmunity(0.0, 1.0, 1.0, 1.0, 0.0, 1.0_yrs);
  EXPECT_EQ(person.getParasiteImmunity(),
            parasite_immunity + maternal_parasite + 2.0);
  EXPECT_EQ(person.getClinicalImmunity(),
            clinical_immunity + maternal_clinical + 2.0);
  EXPECT_EQ(person.getNumHypnozoites(), std::size_t(1));

  // Check age of individual removes maternal immunity.
  person.updateImmunity(0.0, 1.0, 1.0, 1.0, 366.0, 1.0_yrs);
  EXPECT_EQ(person.getParasiteImmunity(), parasite_immunity + 2.0);
  EXPECT_EQ(person.getClinicalImmunity(), clinical_immunity + 2.0);
}

TEST(pvivax, clearInfections) {
  auto test_rho = 0.2;
  auto test_age_0 = 1.0;
  auto test_zeta = 12.5;
  auto clinical_immunity = 12.0;
  auto parasite_immunity = 10.0;
  auto maternal_clinical = 50.0;
  auto maternal_parasite = 5.9;
  auto person = pvibm::PVivax(
      0.0, pvibm::Status::S, parasite_immunity, clinical_immunity,
      maternal_parasite, maternal_clinical, test_zeta, test_rho, test_age_0, 0);

  auto return_type = person.updateInfection(1.0, 0.0);
  EXPECT_EQ(return_type, false);
  return_type = person.updateInfection(100.0, 0.0);
  EXPECT_EQ(return_type, false);

  person.queueInfection(10.0, 1.0, 1.0, 10.0);
  person.queueInfection(15.0, 0.0, 1.0, 10.0);
  EXPECT_EQ(person.getNumHypnozoites(), std::size_t(0));
  person.clearInfectionQueue();
  auto [u, infection_q] = person.copyPrivateData();
  EXPECT_EQ(infection_q.size(), std::size_t(0));
}
