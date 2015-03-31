#ifndef STAN__SERVICES__SAMPLE__INIT_ADAPT_HPP
#define STAN__SERVICES__SAMPLE__INIT_ADAPT_HPP

#include <stan/math/prim/mat/fun/Eigen.hpp>
#include <stan/services/arguments/categorical_argument.hpp>
#include <stan/services/arguments/singleton_argument.hpp>

namespace stan {
  namespace services {
    namespace sample {
    
      template<class Sampler, class ErrWriter>
      bool init_adapt(Sampler& sampler,
                      const double delta,
                      const double gamma,
                      const double kappa, 
                      const double t0,
                      const Eigen::VectorXd& cont_params,
                      ErrWriter& err) {
        const double epsilon = sampler.get_nominal_stepsize();
        
        sampler.get_stepsize_adaptation().set_mu(log(10 * epsilon));
        sampler.get_stepsize_adaptation().set_delta(delta);
        sampler.get_stepsize_adaptation().set_gamma(gamma);
        sampler.get_stepsize_adaptation().set_kappa(kappa);
        sampler.get_stepsize_adaptation().set_t0(t0);
        
        sampler.engage_adaptation();
        
        try {
          sampler.z().q = cont_params;
          sampler.init_stepsize();
        } catch (const std::exception& e) {
          err("Error initializing step size.");
          err(e.what());
          return false;
        }
        
        return true;
      }

      template<class Sampler, class ErrWriter>
      bool init_adapt(Sampler& sampler,
                      stan::services::categorical_argument* adapt,
                      const Eigen::VectorXd& cont_params,
                      ErrWriter& err) {
        
        double delta = dynamic_cast<stan::services::real_argument*>
                       (adapt->arg("delta"))->value();
        double gamma = dynamic_cast<stan::services::real_argument*>
                       (adapt->arg("gamma"))->value();
        double kappa = dynamic_cast<stan::services::real_argument*>
                       (adapt->arg("kappa"))->value();
        double t0    = dynamic_cast<stan::services::real_argument*>
                       (adapt->arg("t0"))->value();
        
        return init_adapt(sampler, delta, gamma, kappa, t0, cont_params, err);
      }

    } // sample
  } // services
} // stan

#endif