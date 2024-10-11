```python
from pypcs.curve import Fp, Fr, ec_mul, G1Point
import random

# Zero-Knowledge Multiply Proof


# TODO: should move to common file
class PedersenParams:
    def __init__(self):
        self.G = G1Point.ec_gen_group1()
        self.H = self.generateH()

    @staticmethod
    def generateH() -> G1Point:
        # generate a random point as H
        r = Fr.rand(random.Random("hash-to-point"))
        return ec_mul(G1Point.ec_gen_group1(), r)


def commit(sk: Fr, rho: Fr, pp: PedersenParams) -> G1Point:
    """
    Computationally hiding
    Statistically binding
    """
    return ec_mul(G1Point.ec_gen_group1(), sk) + ec_mul(pp.H, rho)


class Prover:
    a: Fr
    b: Fr
    c: Fr  # c = a * b
    tau_a: Fr
    tau_b: Fr
    tau_c: Fr

    A: G1Point
    B: G1Point
    C: G1Point  # C = [c;tau_c]

    r_a: Fr
    r_b: Fr
    rho_a: Fr
    rho_b: Fr
    e_0: Fr  # blind for a * b
    e_1: Fr  # blind for r_a * b + r_b * a

    pp: PedersenParams
    rnd_gen: random.Random

    def __init__(self, a: Fr, b: Fr, pp: PedersenParams):
        self.rnd_gen = random.Random("multiply-prover")

        self.a = a
        self.b = b
        self.c = a * b

        self.tau_a = Fr.rand(self.rnd_gen)
        self.tau_b = Fr.rand(self.rnd_gen)
        self.tau_c = Fr.rand(self.rnd_gen)
        self.pp = pp

        self.A = commit(self.a, self.tau_a, self.pp)
        self.B = commit(self.b, self.tau_b, self.pp)
        self.C = commit(self.c, self.tau_c, self.pp)

    def round1(self) -> tuple[G1Point, G1Point, G1Point, G1Point]:
        self.r_a = Fr.rand(self.rnd_gen)
        self.r_b = Fr.rand(self.rnd_gen)
        self.r_c = self.r_a * self.r_b

        self.rho_a = Fr.rand(self.rnd_gen)
        self.rho_b = Fr.rand(self.rnd_gen)
        self.rho_c = Fr.rand(self.rnd_gen)
        self.e_0 = Fr.rand(self.rnd_gen)
        self.e_1 = Fr.rand(self.rnd_gen)

        R_A = commit(self.r_a, self.rho_a, self.pp)
        R_B = commit(self.r_b, self.rho_b, self.pp)
        E_0 = commit((self.r_a * self.r_b), self.e_0, self.pp)
        E_1 = commit((self.r_a * self.b + self.r_b * self.a), self.e_1, self.pp)

        return R_A, R_B, E_0, E_1

    def round3(self, e: Fr) -> tuple[Fr, Fr, Fr, Fr, Fr]:
        z_a = self.r_a + e * self.a
        z_tau_a = self.rho_a + e * self.tau_a

        z_b = self.r_b + e * self.b
        z_tau_b = self.rho_b + e * self.tau_b

        z_tau_c = self.e_0 + e * self.e_1 + e * e * self.tau_c

        return z_a, z_b, z_tau_a, z_tau_b, z_tau_c


class Verifier:
    A: G1Point
    B: G1Point
    C: G1Point

    pp: PedersenParams
    rnd_gen: random.Random

    def __init__(self, A: G1Point, B: G1Point, C: G1Point, pp: PedersenParams):
        self.rnd_gen = random.Random("multiply-verifier")

        self.A = A
        self.B = B
        self.C = C

        self.pp = pp

    def round2(self: G1Point) -> Fr:
        e = Fr.rand(self.rnd_gen)
        return e

    def verify(
        self,
        e: Fr,
        R_A: G1Point,
        R_B: G1Point,
        E_0: G1Point,
        E_1: G1Point,
        z_a: Fr,
        z_b: Fr,
        z_tau_a: Fr,
        z_tau_b: Fr,
        z_tau_c: Fr,
    ) -> bool:
        """
        [z_a;z_tau_a] ?= R_A + A * e
        [z_b;z_tau_b] ?= R_B + B * e
        [z_a*z_b;z_tau_c] ?= E_0 + e*E_1 + e*e*C
        """
        return (
            commit(z_a, z_tau_a, self.pp) == R_A + ec_mul(self.A, e)
            and commit(z_b, z_tau_b, self.pp) == R_B + ec_mul(self.B, e)
            and commit((z_a * z_b), z_tau_c, self.pp)
            == E_0 + ec_mul(E_1, e) + ec_mul(self.C, e * e)
        )


def run_multiply_proof(prover: Prover, verifier: Verifier) -> bool:
    R_A, R_B, E_0, E_1 = prover.round1()
    e = verifier.round2()
    z_a, z_b, z_tau_a, z_tau_b, z_tau_c = prover.round3(e)
    return verifier.verify(e, R_A, R_B, E_0, E_1, z_a, z_b, z_tau_a, z_tau_b, z_tau_c)


def simulate(A: G1Point, B: G1Point, C: G1Point, verifier: Verifier) -> Fr:
    pp = verifier.pp

    # 1 simulator does Round 1 of prover
    # Actually we don't need to do anything, we just need to use verifier's challenge
    # to generate fake the proof

    # 2. simulator invoke Round 2 of verifier
    st = verifier.rnd_gen.getstate()
    e = verifier.round2()

    # 3. simulator does Round 3 of prover
    # 3.1 generate fake z_a
    z_a_star = Fr.rand(random.Random("multiply-proof-z-a"))
    z_tau_a_star = Fr.rand(random.Random("multiply-proof-z-tau-a"))
    Z_A_star = commit(z_a_star, z_tau_a_star, pp)
    R_A_star = Z_A_star - ec_mul(A, e)
    # 3.2 generate fake z_b
    z_b_star = Fr.rand(random.Random("multiply-proof-z-b"))
    z_tau_b_star = Fr.rand(random.Random("multiply-proof-z-tau-a"))
    Z_B_star = commit(z_b_star, z_tau_b_star, pp)
    R_B_star = Z_B_star - ec_mul(B, e)
    # 3.3 generate fake z_c_star
    z_c_star = z_a_star * z_b_star
    z_tau_c_star = Fr.rand(random.Random("multiply-proof-z-tauc-c"))
    Z_C_star = commit(z_c_star, z_tau_c_star, pp)
    E_1_star = ec_mul(
        G1Point.ec_gen_group1(), Fr.rand(random.Random("multiply-proof-e-1"))
    )
    E_0_star = Z_C_star - ec_mul(C, e * e) - ec_mul(E_1_star, e)

    verifier.rnd_gen.setstate(st)
    e_star = verifier.round2()
    assert e == e_star

    return verifier.verify(
        e,
        R_A_star,
        R_B_star,
        E_0_star,
        E_1_star,
        z_a_star,
        z_b_star,
        z_tau_a_star,
        z_tau_b_star,
        z_tau_c_star,
    )


def extract(prover: Prover) -> tuple[Fr, Fr]:
    rng = random.Random("multiply-proof-extract")

    R_A, R_B, E_0, E_1 = prover.round1()

    e_1 = Fr.rand(rng)
    e_2 = Fr.rand(rng)

    z_a_1, z_b_1, z_tau_a_1, z_tau_b_1, z_tau_c_1 = prover.round3(e_1)
    z_a_2, z_b_2, z_tau_a_2, z_tau_b_2, z_tau_c_2 = prover.round3(e_2)

    a = (z_a_2 - z_a_1) / (e_2 - e_1)
    b = (z_b_2 - z_b_1) / (e_2 - e_1)
    tau_a = (z_tau_a_2 - z_tau_a_1) / (e_2 - e_1)
    tau_b = (z_tau_b_2 - z_tau_b_1) / (e_2 - e_1)

    return a, b, tau_a, tau_b


if __name__ == "__main__":
    pedersen_params = PedersenParams()
    a = Fr(31)
    b = Fr(17)
    prover = Prover(a, b, pedersen_params)
    verifier = Verifier(prover.A, prover.B, prover.C, pedersen_params)

    print(f"?: {run_multiply_proof(prover, verifier)}")
    print(f"?: {simulate(prover.A, prover.B, prover.C, verifier)}")
    print(f"?: {extract(prover)}")
```