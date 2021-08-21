import numpy as np
from matplotlib import pyplot as plt
import numba as nb
import sys, mplhep
from iminuit.cost import ExtendedUnbinnedNLL
from iminuit import Minuit
from numba_stats import norm_pdf
from scipy.stats import norm
from time import time

plt.style.use(mplhep.style.ATLAS)

GUI = False
if "plot" in sys.argv:
    GUI = True


@nb.njit(fastmath=True, nogil=True)
def gauss3(x, par):
    return par[0] + par[3] + par[6], par[0] * norm_pdf(x, par[1], par[2]) + par[3] * norm_pdf(x, par[4], par[5]) + par[
        6
    ] * norm_pdf(x, par[7], par[8])


plt.style.use(mplhep.style.ATLAS)

fname = sys.argv[1]

NPEAKS = 3
DPPESTIM = 26
DPPWIDTH = 5

with np.load(fname) as f:
    matrix = f["hg"]

pedestals = np.load("pedestalsHg.npy")

dpps = np.empty_like(pedestals)

if GUI:
    fig, ax = plt.subplots()
for r in range(matrix.shape[0]):
    if r % 4 == 0:
        print()
    for c in range(matrix.shape[1]):
        channeldata = matrix[r, c, :]
        channeldata = channeldata[channeldata > 0] - pedestals[r, c]
        y, x = np.histogram(channeldata, np.arange(-50, 400, 2))
        x = (x[1:] + x[:-1]) * 0.5
        prevMu = DPPESTIM
        prevDpp = DPPESTIM

        fits = []
        for i in range(NPEAKS):
            InfIdx = np.argmax(x > (prevMu + prevDpp - 1.5 * DPPWIDTH))
            SupIdx = np.argmax(x > (prevMu + prevDpp + 1.5 * DPPWIDTH))
            xFit = x[InfIdx:SupIdx]
            yFit = y[InfIdx:SupIdx]
            muEstim = xFit[np.argmax(yFit)]

            dataToFit = channeldata[np.abs(channeldata - muEstim) < 3 * DPPWIDTH]
            sigmaEstim = np.std(dataToFit)

            mu, sigma = norm.fit(dataToFit, loc=muEstim, scale=sigmaEstim)
            prevMu = mu
            aEstim = dataToFit.size / (2 * np.pi * sigma ** 2) ** 0.5
            fits.append((aEstim, mu, sigma))

            if GUI:
                ax.plot(x, 2 * dataToFit.size * norm_pdf(x, mu, sigma), "b")
                ax.vlines(dataToFit.min(), 0, y.max() * 1.3, ls=":", lw=0.8, color="b")
                ax.vlines(dataToFit.max(), 0, y.max() * 1.3, ls=":", lw=0.8, color="b")

        xInf = fits[0][1] - 1.5 * fits[0][2]
        xSup = fits[-1][1] + 1.5 * fits[-1][2]

        dataToFit = channeldata[(channeldata > xInf) & (channeldata < xSup)]

        cost3 = ExtendedUnbinnedNLL(dataToFit, gauss3)
        fit3 = Minuit(cost3, (*fits[0], *fits[1], *fits[2]))
        # A
        fit3.limits[0] = (0, None)
        fit3.limits[3] = (0, None)
        fit3.limits[6] = (0, None)
        # Mu
        fit3.fixed[1] = True
        fit3.fixed[4] = True
        fit3.fixed[7] = True
        # Sigma
        fit3.limits[2] = (0.8 * fits[0][2], 1.2 * fits[0][2])
        fit3.limits[5] = (0.8 * fits[1][2], 1.2 * fits[1][2])
        fit3.limits[8] = (0.8 * fits[2][2], 1.2 * fits[2][2])
        fit3.migrad()
        fit3.fixed = False
        fit3.limits[1] = (0.9 * fit3.values[1], 1.1 * fit3.values[1])
        fit3.limits[4] = (0.9 * fit3.values[4], 1.1 * fit3.values[4])
        fit3.limits[7] = (0.9 * fit3.values[7], 1.1 * fit3.values[7])
        fit3.migrad()
        dpp = (
            (fit3.values[7] - fit3.values[4])
            + (fit3.values[4] - fit3.values[1])
            + (fit3.values[7] - fit3.values[1]) / 2
        ) / 3

        dpps[r, c] = dpp

        if GUI:
            ax.step(x, y, color="k", lw=2, label=(f"Channel of row {r} column {c}"))
            ax.plot(x, 2 * gauss3(x, tuple(fit3.values))[1], "r")
            ax.vlines(dataToFit.min(), 0, y.max() * 1.3, ls=":", lw=1, color="r")
            ax.vlines(dataToFit.max(), 0, y.max() * 1.3, ls=":", lw=1, color="r")
            mplhep.label._exp_label(ax=ax, data=True, exp="IDEA Dual-Readout", rlabel="SiPM Calibration")
            ax.set_yscale("log")
            ax.set_ylim(1, y.max() * 1.3)
            ax.set_xlim(-50, 400)
            plt.legend(
                frameon=False,
                title=f"$\mu_1 = {fit3.values[1]:.2f} +/- {fit3.errors[1]:.2f}$\n"
                f"$\mu_2 = {fit3.values[4]:.2f} +/- {fit3.errors[4]:.2f}$\n"
                f"$\mu_3 = {fit3.values[7]:.2f} +/- {fit3.errors[7]:.2f}$\n",
            )
            plt.draw()
            plt.waitforbuttonpress()
            ax.cla()
        print(f"Channel {r} {c} has DPP {dpps[r][c]}")


if "save" in sys.argv:
    np.save("dppHg", dpps)
