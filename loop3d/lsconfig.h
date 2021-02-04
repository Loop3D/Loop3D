#ifndef LSCONFIG_H
#define LSCONFIG_H

#include <QQuickItem>

static std::string LSConfigSolverName[] = {
    "pyamg",
    "cg",
    "lu"
};
static std::string LSConfigFoliationInterpolatorName[] = {
    "PLI",
    "something else"
};
static std::string LSConfigFaultInterpolatorName[] = {
    "FDI",
    "something else"
};

class LSConfig : public QQuickItem
{
    enum LSConfigSolver {
        PYAMG = 0,
        CG = 1,
        LU = 2
    };
    enum LSConfigFoliationInterpolator {
        PLI = 0,
        FOLIATION_OTHER
    };
    enum LSConfigFaultInterpolator {
        FDI = 0,
        FAULT_OTHER
    };

    Q_OBJECT

    Q_PROPERTY(int    foliationInterpolator MEMBER m_foliationInterpolator NOTIFY foliationInterpolatorChanged)
    Q_PROPERTY(int    foliationNumElements  MEMBER m_foliationNumElements  NOTIFY foliationNumElementsChanged)
    Q_PROPERTY(double foliationBuffer       MEMBER m_foliationBuffer       NOTIFY foliationBufferChanged)
    Q_PROPERTY(int    foliationSolver       MEMBER m_foliationSolver       NOTIFY foliationSolverChanged)
    Q_PROPERTY(int    foliationDamp         MEMBER m_foliationDamp         NOTIFY foliationDampChanged)

    Q_PROPERTY(int    faultInterpolator MEMBER m_faultInterpolator NOTIFY faultInterpolatorChanged)
    Q_PROPERTY(int    faultNumElements  MEMBER m_faultNumElements  NOTIFY faultNumElementsChanged)
    Q_PROPERTY(double faultDataRegion   MEMBER m_faultDataRegion   NOTIFY faultDataRegionChanged)
    Q_PROPERTY(int    faultSolver       MEMBER m_faultSolver       NOTIFY faultSolverChanged)
    Q_PROPERTY(int    faultCpw          MEMBER m_faultCpw          NOTIFY faultCpwChanged)
    Q_PROPERTY(int    faultNpw          MEMBER m_faultNpw          NOTIFY faultNpwChanged)

Q_SIGNALS:
    void foliationInterpolatorChanged();
    void foliationNumElementsChanged();
    void foliationBufferChanged();
    void foliationSolverChanged();
    void foliationDampChanged();

    void faultInterpolatorChanged();
    void faultNumElementsChanged();
    void faultDataRegionChanged();
    void faultSolverChanged();
    void faultCpwChanged();
    void faultNpwChanged();

public:
    LSConfig();
    void reset(void);
    int saveToFile(QString filename = "");
    int loadFromFile(QString filename = "");

    int m_foliationInterpolator;
    int m_foliationNumElements;
    double m_foliationBuffer;
    int m_foliationSolver;
    int m_foliationDamp;

    int m_faultInterpolator;
    int m_faultNumElements;
    double m_faultDataRegion;
    int m_faultSolver;
    int m_faultCpw;
    int m_faultNpw;
};

#endif // LSCONFIG_H
