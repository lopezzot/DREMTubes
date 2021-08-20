import numpy as np
import matplotlib.pyplot as plt
import sys, mplhep
import numba as nb
from iminuit import Minuit
from iminuit.cost import LeastSquares
from iminuit.util import propagate
from functools import partial

plt.style.use(mplhep.style.ATLAS)


@nb.njit(fastmath=True)
def line(x, m, q):
    return x * m + q


GUI = False
if "plot" in sys.argv:
    GUI = True

INF = 10
SUP = 130

fname = sys.argv[1]

with np.load(fname) as f:
    matrixHg = np.float32(f["hg"])
    matrixLg = np.float32(f["lg"])

pedestalHg = np.load("pedestalsHg.npy")
dppHg = np.load("dppHg.npy")

matrixHg[matrixHg == 0] = np.inf
matrixHg = (matrixHg - pedestalHg[:, :, None]) / dppHg[:, :, None]
matrixHg[matrixHg == np.inf] = 0

row, col, evt = matrixHg.shape

dppLg = np.zeros_like(dppHg)
pedestalLg = np.zeros_like(pedestalHg)
if GUI:
    fig, ax = plt.subplots()

for r in range(row):
    for c in range(col):

        channelHgPe = matrixHg[r, c, :]
        channelLgADC = matrixLg[r, c, :]

        x = channelHgPe[(channelHgPe > INF) & (channelHgPe < SUP)]
        y = channelLgADC[(channelHgPe > INF) & (channelHgPe < SUP)]

        lsq = LeastSquares(x, y, np.ones_like(y), line)
        fit = Minuit(lsq, m=1, q=60)
        fit.migrad()
        fit.hesse()
        dppLg[r, c] = fit.values["m"]
        pedestalLg[r, c] = fit.values["q"]

        print(
            f"Channel {r} - {c} ADC/pe: {fit.values[0]:.2f} +/- {fit.errors[0]:.2f} Pedestal: {fit.values[1]:.2f} +/- {fit.errors[1]:.2f}"
        )

        if GUI:
            xFit = np.arange(channelHgPe.min(), channelHgPe.max(), 0.1)
            yFit = line(xFit, *fit.values)
            par_b = np.random.multivariate_normal(fit.values, fit.covariance, size=100)
            y_b = [line(xFit, *p) for p in par_b]
            yFitErr = np.std(y_b, axis=0)
            ax.scatter(channelHgPe, channelLgADC, s=1, c="k", label="Data")
            ax.scatter(x, y, s=3, c="b", label="Data fitted")
            ax.plot(xFit, yFit, "r")
            ax.fill_between(xFit, yFit - 5 * yFitErr, yFit + 5 * yFitErr, facecolor="r", alpha=0.5)
            ax.set_xlim(-10, 175)
            ax.set_ylim(10, 1.3 * yFit.max())
            mplhep.label._exp_label(ax=ax, data=True, exp="IDEA Dual-Readout", rlabel="SiPM Calibration")
            plt.legend(
                frameon=False,
                title=f"$DPP_{{lg}} = {fit.values[0]:.2f} +/- {fit.errors[0]:.2f} ADC/pe$\n"
                f"$Pedestal_{{lg}} = {fit.values[1]:.2f} +/- {fit.errors[1]:.2f} ADC$\n",
            )
            plt.draw()
            plt.waitforbuttonpress()
            ax.cla()

if "save" in sys.argv:
    np.save("pedestalsLg", pedestalLg)
    np.save("dppLg", dppLg)
