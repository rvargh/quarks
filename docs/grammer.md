$$
\begin{align}
    [\text{prog}] &\to [\text{Statement}]^* \\
    [{Statement}] &\to
    \begin{cases}
        \text{exit}([\text{Expression}]); \\
        \text{assign}\space\text{identifier} = [
        \text{Expression}];\\
        \text{if} ([\text{Expression}])[\text{Scope}]\\
        \{[\text{Scope}]^*\}
    \end{cases}
    \\
    \text{[Scope]} & \to \{[\text{Statement}]^*\}\\
    [\text{Expression}] &\to
    \begin{cases}
        [\text{Term}] \\
        [\text{BinaryExpression}]
    \end{cases}\\
    [\text{BinaryExpression}] &\to
    \begin{cases}
        [\text{Expression}] * [\text{Expression}] & \text {precedence} = 1 \\
        [\text{Expression}] / [\text{Expression}] & \text {precedence} = 1 \\
        [\text{Expression}] + [\text{Expression}] & \text {precedence} = 0 \\
        [\text{Expression}] - [\text{Expression}] & \text {precedence} = 0 \\
    \end{cases} \\
    [\text{Term}] &\to
    \begin{cases}
        \text{integer\_literal} \\
        \text{Identifier} \\
        ([\text{Expression}])
    \end{cases}
\end{align}
$$
