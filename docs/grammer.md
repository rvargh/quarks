$$
\begin{align}
    [\text{prog}] &\to [\text{Statement}]^* \\
    [{Statement}] &\to
    \begin{cases}
        \text{exit}(\text{Expression}); \\
        \text{assign}\space\text{identifier} = [
        \text{Expression}];
    \end{cases}
    \\
    [\text{Expression}] &\to
    \begin{cases}
        \text{integer\_literal} \\
        \text{Identifier} \\
        \text{BinaryExpression}
    \end{cases}\\
    [\text{BinaryExpression}] &\to
    \begin{cases}
        [\text{Expression}] * [\text{Expression}] & \text {precedence} = 1 \\
        [\text{Expression}] + [\text{Expression}] & \text {precedence} = 0
    \end{cases}
\end{align}
$$
